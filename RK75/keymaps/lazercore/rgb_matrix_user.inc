#include "utils/indicators.h"
#include "timer.h"

#define LAZERCORE_FROW_FLASH_MS 180
#define LAZERCORE_GLOBAL_FLASH_MAX_MS 480

typedef struct {
    bool     active;
    uint16_t started_at;
} lazercore_led_flash_t;

typedef struct {
    bool     active;
    uint16_t started_at;
    uint16_t duration;
    rgb_t    color;
} lazercore_global_flash_t;

static lazercore_led_flash_t lazercore_frow_flashes[RGB_MATRIX_LED_COUNT];
static lazercore_global_flash_t lazercore_global_flash = {0};

static inline uint8_t lazercore_decay(uint16_t elapsed, uint16_t duration, uint8_t peak) {
    if (elapsed >= duration || duration == 0) {
        return 0;
    }
    uint16_t remaining = duration - elapsed;
    return (uint8_t)(((uint32_t)peak * remaining) / duration);
}

void lazercore_rgb_note_frow_press(uint8_t row, uint8_t col) {
    if (row != 0) {
        return;
    }
    uint8_t led = g_led_config.matrix_co[row][col];
    if (led == NO_LED || led >= RGB_MATRIX_LED_COUNT) {
        return;
    }

    lazercore_frow_flashes[led].active     = true;
    lazercore_frow_flashes[led].started_at = timer_read();
}

bool lazercore_rgb_get_frow_flash(uint8_t led, rgb_t *out) {
    if (led >= RGB_MATRIX_LED_COUNT) {
        return false;
    }

    lazercore_led_flash_t *flash = &lazercore_frow_flashes[led];
    if (!flash->active) {
        return false;
    }

    uint16_t elapsed = timer_elapsed(flash->started_at);
    if (elapsed >= LAZERCORE_FROW_FLASH_MS) {
        flash->active = false;
        return false;
    }

    if (out != NULL) {
        uint8_t intensity = lazercore_decay(elapsed, LAZERCORE_FROW_FLASH_MS, 255);
        out->r            = intensity;
        out->g            = intensity;
        out->b            = intensity;
    }

    return true;
}

static void lazercore_start_global_flash(rgb_t color, uint16_t duration) {
    if (duration > LAZERCORE_GLOBAL_FLASH_MAX_MS) {
        duration = LAZERCORE_GLOBAL_FLASH_MAX_MS;
    }

    lazercore_global_flash.active     = true;
    lazercore_global_flash.started_at = timer_read();
    lazercore_global_flash.duration   = duration;
    lazercore_global_flash.color      = color;
}

bool lazercore_rgb_get_global_flash(rgb_t *out) {
    if (!lazercore_global_flash.active) {
        return false;
    }

    uint16_t elapsed = timer_elapsed(lazercore_global_flash.started_at);
    if (elapsed >= lazercore_global_flash.duration) {
        lazercore_global_flash.active = false;
        return false;
    }

    if (out != NULL) {
        uint8_t fade = lazercore_decay(elapsed, lazercore_global_flash.duration, 255);
        out->r       = (uint8_t)(((uint16_t)lazercore_global_flash.color.r * fade) / 255);
        out->g       = (uint8_t)(((uint16_t)lazercore_global_flash.color.g * fade) / 255);
        out->b       = (uint8_t)(((uint16_t)lazercore_global_flash.color.b * fade) / 255);
    }

    return true;
}

void lazercore_rgb_flash_nkro(void) {
    lazercore_start_global_flash((rgb_t){0x30, 0xE0, 0xFF}, 320);
}

void lazercore_rgb_flash_socd(void) {
    lazercore_start_global_flash((rgb_t){0xFF, 0x66, 0xCC}, 320);
}

void lazercore_rgb_flash_dfu(void) {
    lazercore_start_global_flash((rgb_t){0xFF, 0x80, 0x40}, 420);
}

void lazercore_rgb_flash_eeprom(void) {
    lazercore_start_global_flash((rgb_t){0xFF, 0xD4, 0x40}, 420);
}

