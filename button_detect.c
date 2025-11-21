#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>

#define CHIP "/dev/gpiochip4"  // GPIO chip device
#define LINE 9                // GPIO line number for button

int main() {
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    struct gpiod_line_event event;
    int ret;

    // Open GPIO chip
    chip = gpiod_chip_open(CHIP);
    if (!chip) {
        perror("gpiod_chip_open");
        return EXIT_FAILURE;
    }

    // Get GPIO line
    line = gpiod_chip_get_line(chip, LINE);
    if (!line) {
        perror("gpiod_chip_get_line");
        gpiod_chip_close(chip);
        return EXIT_FAILURE;
    }

    // Request line as input with both edge detection
    ret = gpiod_line_request_both_edges_events(line, "button_detect");
    if (ret < 0) {
        perror("gpiod_line_request_both_edges_events");
        gpiod_chip_close(chip);
        return EXIT_FAILURE;
    }

    printf("Waiting for button press/release on line %d...\n", LINE);

    // Event loop
    while (1) {
        ret = gpiod_line_event_wait(line, NULL); // Wait indefinitely
        if (ret < 0) {
            perror("gpiod_line_event_wait");
            break;
        } else if (ret == 0) {
            continue; // Timeout (not used here)
        }

        // Read event
        ret = gpiod_line_event_read(line, &event);
        if (ret < 0) {
            perror("gpiod_line_event_read");
            break;
        }

        if (event.event_type == GPIOD_LINE_EVENT_RISING_EDGE) {
            printf("Button PRESSED!\n");
        } else if (event.event_type == GPIOD_LINE_EVENT_FALLING_EDGE) {
            printf("Button RELEASED!\n");
        }
    }

    gpiod_chip_close(chip);
    return EXIT_SUCCESS;
}