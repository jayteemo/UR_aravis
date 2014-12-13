#include <arvgvspsocketprivate.h>
#include <arvgvcp.h>
#include <sys/socket.h>

typedef enum {
	ARV_GVSP_SOCKET_TYPE_GLIB,
	ARV_GVSP_SOCKET_TYPE_PACKET
} ArvSocketType;

struct _ArvGvspSocket {
	ArvSocketType type;
	int fd;
	union {
		struct {
			GSocket *socket;
			GPollFD poll_fd;
			int current_buffer_size;
			GSocketAddress *device_address;
		} glib;
		struct {
		} packet;
	};
};

ArvGvspSocket *
arv_gvsp_socket_new (GInetAddress *device_address)
{
	ArvGvspSocket *gvsp_socket;
	GInetAddress *incoming_inet_address;
	GSocketAddress *incoming_address;

	gvsp_socket = g_new0 (ArvGvspSocket, 1);
	gvsp_socket->type = ARV_GVSP_SOCKET_TYPE_GLIB;

	gvsp_socket->glib.socket = g_socket_new (G_SOCKET_FAMILY_IPV4,
						 G_SOCKET_TYPE_DATAGRAM,
						 G_SOCKET_PROTOCOL_UDP, NULL);

	incoming_inet_address = g_inet_address_new_any (G_SOCKET_FAMILY_IPV4);
	incoming_address = g_inet_socket_address_new (incoming_inet_address, 0);
	g_object_unref (incoming_inet_address);

	g_socket_bind (gvsp_socket->glib.socket, incoming_address, TRUE, NULL);

	g_object_unref (incoming_address);

	gvsp_socket->fd = g_socket_get_fd (gvsp_socket->glib.socket);
	gvsp_socket->glib.poll_fd.fd = gvsp_socket->fd;
	gvsp_socket->glib.poll_fd.events = G_IO_IN;
	gvsp_socket->glib.poll_fd.revents = 0;
	gvsp_socket->glib.device_address = g_inet_socket_address_new (device_address, ARV_GVCP_PORT);
	gvsp_socket->glib.current_buffer_size = 0;

	return gvsp_socket;
} 

void
arv_gvsp_socket_free (ArvGvspSocket *gvsp_socket)
{
	g_return_if_fail (gvsp_socket != NULL);

	g_object_unref (gvsp_socket->glib.socket);
	g_object_unref (gvsp_socket->glib.device_address);
	g_free (gvsp_socket);
}

gint16
arv_gvsp_socket_get_port (ArvGvspSocket *gvsp_socket)
{
	GInetSocketAddress *local_address;
	guint16 port;

	g_return_val_if_fail (gvsp_socket != NULL, 0);

	local_address = G_INET_SOCKET_ADDRESS (g_socket_get_local_address (gvsp_socket->glib.socket, NULL));
	port = g_inet_socket_address_get_port (local_address);
	g_object_unref (local_address);

	return port;
}

int
arv_gvsp_socket_poll (ArvGvspSocket *gvsp_socket, int timeout_ms)
{
	g_return_val_if_fail (gvsp_socket != NULL, 0);

	return g_poll (&gvsp_socket->glib.poll_fd, 1, timeout_ms);
}

gsize
arv_gvsp_socket_receive (ArvGvspSocket *gvsp_socket, char *buffer, gsize size)
{
	g_return_val_if_fail (gvsp_socket != NULL, 0);

	return g_socket_receive (gvsp_socket->glib.socket, buffer, size, NULL, NULL);
}

gboolean
arv_gvsp_socket_send_to (ArvGvspSocket *gvsp_socket, const char *data, gsize size)
{
	g_return_val_if_fail (gvsp_socket != NULL, FALSE);

	return g_socket_send_to (gvsp_socket->glib.socket, gvsp_socket->glib.device_address, data, size, NULL, NULL) >= 0;
}

gboolean
arv_gvsp_socket_set_buffer_size	(ArvGvspSocket *gvsp_socket, gsize buffer_size)
{
	g_return_val_if_fail (gvsp_socket != NULL, FALSE);

	if (buffer_size != gvsp_socket->glib.current_buffer_size) {
		setsockopt (gvsp_socket->fd, SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof (buffer_size));
		gvsp_socket->glib.current_buffer_size = buffer_size;
	}

	return TRUE;
}
