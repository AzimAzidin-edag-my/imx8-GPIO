#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define CHIP "/dev/gpiochip0"  // GPIO chip device
#define LINE 23                // GPIO line number for LED

int main() {
    struct gpiod_chip *chip;
    struct gpiod_line *line;
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

    // Request line as output, initial value 0 (LED OFF)
    ret = gpiod_line_request_output(line, "led_control", 0);
    if (ret < 0) {
        perror("gpiod_line_request_output");
        gpiod_chip_close(chip);
        return EXIT_FAILURE;
    }

    // Turn LED ON
    printf("LED ON\n");
    gpiod_line_set_value(line, 1);
    sleep(2);

    // Turn LED OFF
    printf("LED OFF\n");
    gpiod_line_set_value(line, 0);

    // Release resources
    gpiod_chip_close(chip);
    return EXIT_SUCCESS;
}
