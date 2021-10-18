#if 0
typedef struct _SAPPacket
{
    guint8      SAPVersion;
    gboolean    AddressType;
    gboolean    MessageType;
    gboolean    Encryption;
    gboolean    Compression;
    guint8      AuthenticationLength;
    guint16     MessageIdentifierHash;
    gchar      *OriginatingSourceAddress;
    gchar      *PayloadType;
    gchar      *SDPDescription;
}
    SAPPacket;
SAPPacket* convertSAPStringToStruct(gchar *SAPString)
{
    SAPPacket *ReturnPacket = g_malloc0(sizeof(SAPPacket));

    // *** Refer to RFC 2974 for details on the header parts extracted below ***

    // 8 first bits are the flags
    guint8 PacketFlags = SAPString[0];

    // First two bits are useless...                   7
    // Because reasons.                                6
    ReturnPacket->SAPVersion    = GET_BIT(PacketFlags, 5);
    ReturnPacket->AddressType   = GET_BIT(PacketFlags, 4);
    // Fifth bit is reserved, therefore useless to us. 3
    ReturnPacket->MessageType   = GET_BIT(PacketFlags, 2);
    ReturnPacket->Encryption    = GET_BIT(PacketFlags, 1);
    ReturnPacket->Compression   = GET_BIT(PacketFlags, 0);

    // 8 following bits give an unsigned integer
    // for the authentication header length
    ReturnPacket->AuthenticationLength = (guint8) SAPString[1];

    // 16 following bits are a unique hash attached to the stream
    ReturnPacket->MessageIdentifierHash =
        (guint16) concatenateBytes((guint8 *) SAPString, 2, 3);

    // If AddressType is IPv4, the following 32 bits give the IPv4 address.
    // Otherwise if it's IPv6, the following 128 bits give the IPv6 address.
    // But since only IPv4 is supported for now, we will not consider IPv6.
    // An error will be yielded when checking this struct if it comes with IPv6.
    gsize AddressEndingByte = 0;
    GSocketFamily AddressFamily;

    if(ReturnPacket->AddressType == SAP_SOURCE_IS_IPV4)
    {
        AddressEndingByte = 7;
        AddressFamily = G_SOCKET_FAMILY_IPV4;
    }
    else if(ReturnPacket->AddressType == SAP_SOURCE_IS_IPV6)
    {
        AddressEndingByte = 19;
        AddressFamily = G_SOCKET_FAMILY_IPV6;
    }

    GInetAddress *SourceAddress =
        g_inet_address_new_from_bytes
        (
            (guint8*) &SAPString[4],
            AddressFamily
        );

    ReturnPacket->OriginatingSourceAddress = g_inet_address_to_string
                                                (SourceAddress);

    g_clear_object(&SourceAddress);

    // Authentication header is skipped, because it does not look like it's
    // used in AES67 SAP announcements.

    // Payload type is a MIME type, "application/sdp" in our case
    gsize PayloadTypeStartIndex =
        AddressEndingByte + ReturnPacket->AuthenticationLength + 1;

    ReturnPacket->PayloadType = g_strdup(&SAPString[PayloadTypeStartIndex]);

    // Now we can gather the actual SDP description
    ReturnPacket->SDPDescription =
        g_strdup
        (
            &SAPString
            [PayloadTypeStartIndex + strlen(ReturnPacket->PayloadType) + 1]
        );

    return ReturnPacket;
}
gboolean checkSAPPacket(SAPPacket *PacketToCheck)
{
    if
    (
        PacketToCheck->SAPVersion != 1 ||
        PacketToCheck->AddressType != SAP_SOURCE_IS_IPV4 ||
        PacketToCheck->Encryption != 0 ||
        PacketToCheck->Compression != 0 ||
        g_strcmp0(PacketToCheck->PayloadType, "application/sdp")
    )
        return FALSE;
    else
        return TRUE;
}
void updateSAPTable(sqlite3 *SDPDatabase, SAPPacket *PacketToProcess)
{
    if(checkSAPPacket(PacketToProcess))
    {
        if(PacketToProcess->MessageType == SAP_ANNOUNCEMENT_PACKET)
            insertSAPPacketInSAPTable(SDPDatabase, PacketToProcess);
        else if(PacketToProcess->MessageType == SAP_DELETION_PACKET)
            removeSAPPacketFromSAPTable(SDPDatabase, PacketToProcess);
    }
    else
        g_debug
        (
            "Invalid SAP Packet. Ignoring...\n\tID = 0x%04X",
            PacketToProcess->MessageIdentifierHash
        );
}
#endif