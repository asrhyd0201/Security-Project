/* GUI Program*/
#include<gtk/gtk.h>
#include<string.h>
#include"mquery.h"

GtkWidget *window;
GtkWidget *fixed;

GtkWidget *expert_label;
GtkWidget *expert_tv;
GtkWidget *mailid_label,*result_label;
GtkWidget *mailid_entry;
GtkWidget *payload_button,*mail_button,*close_button;
GtkWidget *clear_button;
GtkTextBuffer *buffer;

void mail_clicked(GtkWidget *widget, gpointer data)
{
  	gchar *s;
	int i;
	s=gtk_entry_get_text(GTK_ENTRY(mailid_entry));
	i=findMail(s);
	if(i==1)
		gtk_label_set_text(GTK_LABEL(result_label),"Result : Mail Id found");
	else
		gtk_label_set_text(GTK_LABEL(result_label),"Result : Mail Id not found");
}

void clear(GtkWidget *widget, gpointer data)
{
	GtkTextIter start,end;
  	gtk_entry_set_text(GTK_ENTRY(mailid_entry),"");
	//gtk_text_buffer_set_text(buffer,"\0",strlen(""));
	gtk_text_buffer_get_start_iter(buffer,&start);
	gtk_text_buffer_get_end_iter(buffer,&end);
	gtk_text_buffer_delete(buffer,&start,&end);
	gtk_label_set_text(GTK_LABEL(result_label),"Result :");
}
void payload(GtkWidget *widget, gpointer data)
{
	gchar *s;
	int i;
	GtkTextIter start,end;
	gtk_text_buffer_get_start_iter(buffer,&start);
	gtk_text_buffer_get_end_iter(buffer,&end);
	s=gtk_text_buffer_get_text(buffer,&start,&end,FALSE);
	printf("%s\n",s);
	i=findPayload(s);
	printf("%d\n",i);
	if(i==1)
		gtk_label_set_text(GTK_LABEL(result_label),"Result : Excerpt of Mail found");
	else
		gtk_label_set_text(GTK_LABEL(result_label),"Result : Excerpt of Mail not found");
}
int main( int argc, char *argv[])
{
		

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Bloom Filter");
	gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	
	fixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), fixed);

	expert_label = gtk_label_new("Enter Excerpt of mail");
	gtk_fixed_put(GTK_FIXED(fixed), expert_label, 15, 5);

	expert_tv = gtk_text_view_new();
	//gtk_text_view_set_buffer(GTK_TEXT_VIEW(expert_tv),buffer);
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(expert_tv));
	gtk_fixed_put(GTK_FIXED(fixed), expert_tv, 15, 30);	
	gtk_widget_set_size_request(expert_tv, 450, 300);

	mailid_label = gtk_label_new("Enter Mail id");
	gtk_fixed_put(GTK_FIXED(fixed), mailid_label, 15, 340);

	mailid_entry = gtk_entry_new();
	gtk_fixed_put(GTK_FIXED(fixed), mailid_entry, 15, 360);
	gtk_widget_set_size_request(mailid_entry, 450, 35);	

	result_label = gtk_label_new("Result : ");
	gtk_fixed_put(GTK_FIXED(fixed), result_label, 15, 410);	

	payload_button = gtk_button_new_with_label("Payload");
	gtk_fixed_put(GTK_FIXED(fixed), payload_button, 15, 450);
	gtk_widget_set_size_request(payload_button, 80, 35);

	mail_button = gtk_button_new_with_label("Mail Id");
	gtk_fixed_put(GTK_FIXED(fixed), mail_button, 120, 450);
	gtk_widget_set_size_request(mail_button, 80, 35);

	clear_button = gtk_button_new_with_label("Clear");
	gtk_fixed_put(GTK_FIXED(fixed), clear_button, 250, 450);
	gtk_widget_set_size_request(clear_button, 80, 35);	
		
	close_button = gtk_button_new_with_label("Close");
	gtk_fixed_put(GTK_FIXED(fixed), close_button, 385, 450);
	gtk_widget_set_size_request(close_button, 80, 35);


	g_signal_connect_swapped(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	g_signal_connect(G_OBJECT(payload_button), "clicked", G_CALLBACK(payload), NULL);	
	
	g_signal_connect(G_OBJECT(mail_button), "clicked", G_CALLBACK(mail_clicked), NULL);

	g_signal_connect(G_OBJECT(clear_button), "clicked", G_CALLBACK(clear), NULL);

	g_signal_connect(G_OBJECT(close_button), "clicked", G_CALLBACK(gtk_main_quit), NULL);


	gtk_widget_show_all(window);

	gtk_main();


	return 0;
}

	

