#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <gst/gst.h>

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 256

#define GPIO_LED 216

int gpio_export(unsigned int gpio)
{
    int fd, len;
    char buf[MAX_BUF];

    fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
    if (fd < 0) {
        perror("gpio/export");
        return fd;
    }

    len = snprintf(buf, sizeof(buf), "%d", gpio);
    write(fd, buf, len);
    close(fd);

    return 0;
}

int gpio_unexport(unsigned int gpio)
{
    int fd, len;
    char buf[MAX_BUF];

    fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
    if (fd < 0) {
        perror("gpio/export");
        return fd;
    }

    len = snprintf(buf, sizeof(buf), "%d", gpio);
    write(fd, buf, len);
    close(fd);
    return 0;
}

int gpio_set_dir(unsigned int gpio, unsigned int out_flag)
{
    int fd, len;
    char buf[MAX_BUF];

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio/direction");
        return fd;
    }

    if (out_flag)
    {
        write(fd, "out", 4);
    }
    else
    {
        write(fd, "in", 3);
    }

    close(fd);
    return 0;
}

int gpio_set_value(unsigned int gpio, unsigned int value)
{
    int fd, len;
    char buf[MAX_BUF];

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio/set-value");
        return fd;
    }

    if (value)
        write(fd, "1", 2);
    else
        write(fd, "0", 2);

    close(fd);
    return 0;
}

int gpio_get_value(unsigned int gpio, unsigned int *value)
{
    int fd, len;
    char buf[MAX_BUF];
    char ch;

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

    fd = open(buf, O_RDONLY);
    if (fd < 0) {
        perror("gpio/get-value");
        return fd;
    }

    read(fd, &ch, 1);

    if (ch != '0') {
        *value = 1;
    } else {
        *value = 0;
    }

    close(fd);
    return 0;
}

int gpio_fd_open(unsigned int gpio)
{
    int fd, len;
    char buf[MAX_BUF];

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

    fd = open(buf, O_RDONLY | O_NONBLOCK );
    if (fd < 0) {
        perror("gpio/fd_open");
    }
    return fd;
}

int gpio_fd_close(int fd)
{
    return close(fd);
}

static gboolean gst_bus_call(GstBus *bus, GstMessage *msg, gpointer data)
{
    GMainLoop *loop = (GMainLoop *)data;
    switch(GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_EOS:
        g_print("End of stream\n");
        g_main_loop_quit(loop);
        break;

    case GST_MESSAGE_ERROR:
    {
        gchar  *debug;
        GError *error;

        gst_message_parse_error(msg, &error, &debug);
        g_free(debug);

        g_printerr("Error: %s\n", error->message);
        g_error_free(error);

        g_main_loop_quit(loop);
        break;
    }
    default:
        break;
    }

    return TRUE;
}

int main(int argc, char *argv[])
{
    GstElement *pipeline;
    GError *err = NULL;
    GstBus *bus;
    GMainLoop *loop;
    const int bufsize = 4096;
    int buflen;
    char buf[bufsize];
    char *cam = "cam1";
    char *out = "0";

    /* int gpio = GPIO_LED; */
    /* gpio_export(gpio); */
    /* gpio_set_dir(gpio, 1); */
    /* gpio_set_value(gpio, 1); */
    
    if(argc > 1)
    {
        cam = argv[1];
    }
    if(argc > 2)
    {
        out = argv[2];
    }

    buflen = snprintf(buf,
                      bufsize,
                      "v4l2src device=/dev/%s io-mode=2 "
                      "! image/jpeg,framerate=(fraction)30/1,width=1920,height=1080 "
                      "! nvjpegdec "
                      "! video/x-raw "
                      "! nvvidconv "
                      "! video/x-raw "
                      "! nvoverlaysink display-id=%s sync=false -ev",
                      cam, out);
    printf("pipeline: %s\n", buf);
    gst_init(&argc, &argv);

    loop = g_main_loop_new(NULL, FALSE);
    pipeline = gst_parse_launch(buf, &err);
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    
    bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch (bus, gst_bus_call, loop);

    g_main_loop_run(loop);

    /* gpio_set_value(gpio, 0); */
    
    return 0;
}
