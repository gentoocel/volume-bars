#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VOLUME 100
#define NUM_BARS 10
#define VOLUME_STEP (MAX_VOLUME / NUM_BARS)

GtkWidget *label;
int previous_volume = -1; // Store previous volume for mute/unmute
int use_pulseaudio = 0;   // 0 for PipeWire, 1 for PulseAudio

// Set the system volume
void set_volume(int volume) {
    char command[128];
    if (use_pulseaudio) {
        snprintf(command, sizeof(command), "pactl set-sink-volume @DEFAULT_SINK@ %d%%", volume);
    } else {
        snprintf(command, sizeof(command), "wpctl set-volume @DEFAULT_AUDIO_SINK@ %d%%", volume);
    }
    system(command);
}

// Get the current system volume
int get_volume() {
    FILE *fp;
    char output[1035];
    int volume = 0;

    if (use_pulseaudio) {
        fp = popen("pactl get-sink-volume @DEFAULT_SINK@ | grep -oP '\\d+%' | head -n 1", "r");
    } else {
        fp = popen("wpctl get-volume @DEFAULT_AUDIO_SINK@ | grep -oP '\\d+\\.\\d+'", "r");
    }

    if (fp == NULL) {
        return 0; // Error: unable to get volume
    }

    if (fgets(output, sizeof(output) - 1, fp) != NULL) {
        volume = use_pulseaudio ? atoi(output) : (int)(atof(output) * 100);
    }

    pclose(fp);
    return volume;
}

// Update the GTK label with volume bars
void update_label(int volume) {
    int filled_bars = (volume * NUM_BARS) / MAX_VOLUME;

    char bar_text[NUM_BARS + 1];
    for (int i = 0; i < NUM_BARS; i++) {
        bar_text[i] = (i < filled_bars) ? '|' : ' ';
    }
    bar_text[NUM_BARS] = '\0';

    gtk_label_set_text(GTK_LABEL(label), bar_text);
}

// Handle key press events for volume control and exiting
gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    int volume = get_volume();

    switch (event->keyval) {
        case GDK_KEY_Right:
        case GDK_KEY_plus:  // '+' key
            volume = (volume + VOLUME_STEP <= MAX_VOLUME) ? volume + VOLUME_STEP : MAX_VOLUME;
            break;
        case GDK_KEY_Left:
        case GDK_KEY_minus:  // '-' key
            volume = (volume - VOLUME_STEP >= 0) ? volume - VOLUME_STEP : 0;
            break;
        case GDK_KEY_m: // Mute/unmute
            if (volume > 0) {
                previous_volume = volume;
                volume = 0;
            } else if (previous_volume != -1) {
                volume = previous_volume;
            }
            break;
        case GDK_KEY_Escape: // Exit program
            gtk_main_quit();
            return FALSE;
        default:
            return FALSE;  // Do nothing for other keys
    }

    set_volume(volume);
    update_label(volume);
    return FALSE;
}

// Close the program when it loses focus
gboolean on_focus_out(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    gtk_main_quit();
    return FALSE;
}

// Apply CSS styling to the label
void apply_css(GtkWidget *widget) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "#volume-label {"
        "   font-weight: bold;"
        "   font-size: 24px;"
        "   color: white;"
        "}", -1, NULL);

    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref(provider);
}

// Detect if PulseAudio or PipeWire is running
void detect_audio_server() {
    if (system("pactl info > /dev/null 2>&1") == 0) {
        use_pulseaudio = 1;
    } else {
        use_pulseaudio = 0;
    }
}

// Main function to initialize GTK and setup the application
int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *box;

    gtk_init(&argc, &argv);

    detect_audio_server();

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Volume Bars!");
    gtk_window_set_default_size(GTK_WINDOW(window), 135, 50);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    label = gtk_label_new("");
    gtk_widget_set_name(label, "volume-label");
    gtk_container_add(GTK_CONTAINER(box), label);

    apply_css(label);
    update_label(get_volume());

    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), NULL);
    g_signal_connect(window, "focus-out-event", G_CALLBACK(on_focus_out), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

