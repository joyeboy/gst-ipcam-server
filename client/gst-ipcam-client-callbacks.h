/*
 * \file:   gst-ipcam-client-callbacks.h
 * \author: Dam Quang Tuan <damquang.tuan@nomovok.com>
 *
 * \date 8-26-2009
 */
#include <gtk/gtk.h>

/*store the url in the current session*/
static gchar *URL;
/**
 * Handle the event when clicking on Connect button in the main window.
 * The button will change to Disconnect button and the connection dialog will appear
 *
 * @param button GtkButton *
 * @param user_data gpointer
 *
 * @return nothing
 */
void
gst_ipcam_client_on_btn_connect_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

/**
 * Handle the event when clicking on Disconnect button in the main window.
 * The button will change to connect button and the application will stop playing
 * the receiving video
 *
 * @param button GtkButton *
 * @param user_data gpointer
 *
 * @return nothing
 */
void
gst_ipcam_client_on_btn_disconnect_clicked              (GtkButton       *button,
                                        gpointer         user_data);

/**
 * Handle the event when clicking on the Pause button in the main window
 * The button will change to Resume button and  the application will pause playing
 * the receving video
 *
 * @param button GtkButton *
 * @param user_data gpointer
 *
 * @return nothing
 */
void
gst_ipcam_client_on_btn_pause_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

/**
 * Handle the event when clicking on the Resume button in the main window
 * The button will change to Pause button and the application will Resume playing
 * the receving video
 *
 * @param button GtkButton *
 * @param user_data gpointer
 *
 * @return nothing
 */
void
gst_ipcam_client_on_btn_resume_clicked                  (GtkButton       *button,
                                        gpointer         user_data);


/**
 * Handle the event when clicking on the About button
 * Show the About dialog
 *
 * @param button GtkButton *
 * @param user_data gpointer
 *
 * @return nothing
 */
void
gst_ipcam_client_on_btn_about_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

/**
 * Handle the event when clicking on the quit button
 * free all memories and close the application
 *
 * @param argc GtkButton *
 * @param user_data gpointer
 *
 * @return nothing
 */
void
gst_ipcam_client_on_btn_quit_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

/**
 * Handle the event when clicking on the connect button in the connection dialog.
 * Get the url and Video stream type, close the dialog and play video
 *
 * @param button GtkButton *
 * @param user_data gpointer
 *
 * @return nothing
 */
void
gst_ipcam_client_on_btn_connect_dialog_clicked           (GtkButton       *button,
                                        gpointer         user_data);

/**
 * Handle the event when user close the connection dialog
 * The application will change the Disconnect button to connect button
 *
 * @param argc GtkButton *
 * @param user_data gpointer
 *
 * @return nothing
 */
void
gst_ipcam_client_on_connection_dialog_destroy                (GtkObject       *object,
                                        gpointer         user_data);

/**
 * Handle the event when user close the main window
 * The application will exit and send Teardown to server
 *
 * @param argc GtkButton *
 * @param user_data gpointer
 *
 * @return nothing
 */
void gst_ipcam_client_on_main_window_destroy                (GtkObject       *object,
                                        gpointer         user_data);

/**
 * Handle the event when users press enter button in the connection_dialog
 * The application will exit and send Teardown to server
 *
 * @param argc GtkButton *
 * @param user_data gpointer
 *
 * @return nothing
 */
gboolean
gst_ipcam_client_on_connection_dialog_key_press_event        (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data);

/**
 * Handle the event when clicking on the Change button in the main window.
 * the video will change fps and bitrate followed the input from users
 *
 * @param button GtkButton *
 * @param user_data gpointer
 *
 * @return nothing
 */
void
gst_ipcam_client_on_btn_connect_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

/**
 * Handle the event when clicking on the Change button in the main window.
 * the video will change fps and bitrate followed the input from users
 *
 * @param button GtkButton *
 * @param user_data gpointer
 *
 * @return nothing
 */
void
gst_ipcam_client_on_btn_change_clicked                 (GtkButton       *button,
                                        gpointer         user_data);