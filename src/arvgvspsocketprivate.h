#ifndef ARV_GVSP_SOCKET_PRIVATE_H
#define ARV_GVSP_SOCKET_PRIVATE_H

#include <arvtypes.h>
#include <gio/gio.h>

typedef struct _ArvGvspSocket		ArvGvspSocket;

ArvGvspSocket * 		arv_gvsp_socket_new 		(GInetAddress *device_address);
void 				arv_gvsp_socket_free 		(ArvGvspSocket *gvsp_socket);
int				arv_gvsp_socket_poll		(ArvGvspSocket *gvsp_socket, int timeout_ms);
gsize 				arv_gvsp_socket_receive 	(ArvGvspSocket *gvsp_socket, char *buffer, gsize size);
gboolean 			arv_gvsp_socket_send_to 	(ArvGvspSocket *gvsp_socket, const char *data, gsize size);
gboolean			arv_gvsp_socket_set_buffer_size	(ArvGvspSocket *gvsp_socket, gsize size);
gint16 				arv_gvsp_socket_get_port 	(ArvGvspSocket *gvsp_socket);

#endif
