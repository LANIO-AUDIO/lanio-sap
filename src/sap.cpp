#include "sap.hpp"

namespace SAP
{
    Receiver::Receiver(asio::io_context& ioContext)
    :   m_socket{ ioContext },
        m_endpoint{ asio::ip::make_address_v4("0.0.0.0"), 9875 },
        m_packetBuffer{}
    {
        m_socket.open(m_endpoint.protocol());
        m_socket.set_option(asio::ip::udp::socket::reuse_address(true));
        m_socket.bind(m_endpoint);

        m_socket.set_option
        (
            asio::ip::multicast::join_group
                (asio::ip::make_address_v4("239.255.255.255"))
        );
    }

    packet_buffer_t Receiver::syncReceive()
    {
        m_socket.receive_from(asio::buffer(m_packetBuffer), m_endpoint);

        Parser parser{ m_packetBuffer };

        return m_packetBuffer;
    }
}
