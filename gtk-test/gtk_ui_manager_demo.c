#include <gtk/gtk.h>

void itemPressed(GtkMenuItem *menuItem, gpointer data) {
    g_print("%s\n", gtk_action_get_name(GTK_ACTION(menuItem)));
}

static GtkActionEntry entries[] = {
  { "File", NULL, "_File" },
  { "Open", GTK_STOCK_OPEN, "Open", 
           "<control>O", "Open File", G_CALLBACK(itemPressed)},
  { "Save", GTK_STOCK_SAVE, "Save", 
           "<control>S", "Save File", G_CALLBACK(itemPressed)},
  { "Close", GTK_STOCK_QUIT, "Close", 
           "<control>Q", "Close File", G_CALLBACK(gtk_main_quit)},
  { "Edit", NULL, "_Edit" },
  { "Cut", NULL, "Cut"},
  { "Copy", NULL, "Copy"},
  { "Paste", NULL, "Paste"},
  { "Help", NULL, "_Help" },
  { "About", NULL, "About" }
};

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkActionGroup *actionGroup;
    GtkUIManager *ui;
    GtkWidget *vbox;
    guint ret;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GtkUIManager");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    actionGroup = gtk_action_group_new("Actions");
    gtk_action_group_add_actions(actionGroup, entries, 10, NULL);
   
    ui = gtk_ui_manager_new();
    gtk_ui_manager_insert_action_group(ui, actionGroup, 0);
    ret = gtk_ui_manager_add_ui_from_file(ui, "./gtk-test/gtk_ui_manager.xml", NULL);
    if (ret == 0) {
		g_print("gtk_ui_manager_add_ui_from_file error!\n");
		return -1;
	}
    
    vbox = gtk_vbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), 
          gtk_ui_manager_get_widget(ui, "/MenuBar"), FALSE, FALSE, 2);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    g_signal_connect(GTK_OBJECT(window), "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}


