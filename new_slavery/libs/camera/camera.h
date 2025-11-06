#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "flash.h"
#include "ssdv.h"
#include "main.h"


#define CS_LOW()  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET) // Assert CS (Active Low)
#define CS_HIGH() HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET)   // Deassert CS (Inactive High)

#define MAX_IMAGE_SIZE 15000



typedef enum {
    CAM_BRIGHTNESS_LEVEL_MINUS_4 = 8, /**<Level -4 */
    CAM_BRIGHTNESS_LEVEL_MINUS_3 = 6, /**<Level -3 */
    CAM_BRIGHTNESS_LEVEL_MINUS_2 = 4, /**<Level -2 */
    CAM_BRIGHTNESS_LEVEL_MINUS_1 = 2, /**<Level -1 */
    CAM_BRIGHTNESS_LEVEL_DEFAULT = 0, /**<Level Default*/
    CAM_BRIGHTNESS_LEVEL_1       = 1, /**<Level +1 */
    CAM_BRIGHTNESS_LEVEL_2       = 3, /**<Level +2 */
    CAM_BRIGHTNESS_LEVEL_3       = 5, /**<Level +3 */
    CAM_BRIGHTNESS_LEVEL_4       = 7, /**<Level +4 */
} CAM_BRIGHTNESS_LEVEL;



typedef enum {
    CAM_SHARPNESS_LEVEL_AUTO = 0, /**<Sharpness Auto */
    CAM_SHARPNESS_LEVEL_1,        /**<Sharpness Level 1 */
    CAM_SHARPNESS_LEVEL_2,        /**<Sharpness Level 2 */
    CAM_SHARPNESS_LEVEL_3,        /**<Sharpness Level 3 */
    CAM_SHARPNESS_LEVEL_4,        /**<Sharpness Level 4 */
    CAM_SHARPNESS_LEVEL_5,        /**<Sharpness Level 5 */
    CAM_SHARPNESS_LEVEL_6,        /**<Sharpness Level 6 */
    CAM_SHARPNESS_LEVEL_7,        /**<Sharpness Level 7 */
    CAM_SHARPNESS_LEVEL_8,        /**<Sharpness Level 8 */
} CAM_SHARPNESS_LEVEL;


typedef enum {
    CAM_CONTRAST_LEVEL_MINUS_3 = 6, /**<Level -3 */
    CAM_CONTRAST_LEVEL_MINUS_2 = 4, /**<Level -2 */
    CAM_CONTRAST_LEVEL_MINUS_1 = 2, /**<Level -1 */
    CAM_CONTRAST_LEVEL_DEFAULT = 0, /**<Level Default*/
    CAM_CONTRAST_LEVEL_1       = 1, /**<Level +1 */
    CAM_CONTRAST_LEVEL_2       = 3, /**<Level +2 */
    CAM_CONTRAST_LEVEL_3       = 5, /**<Level +3 */
} CAM_CONTRAST_LEVEL;


typedef enum {
    CAM_EV_LEVEL_MINUS_3 = 6, /**<Level -3 */
    CAM_EV_LEVEL_MINUS_2 = 4, /**<Level -2 */
    CAM_EV_LEVEL_MINUS_1 = 2, /**<Level -1 */
    CAM_EV_LEVEL_DEFAULT = 0, /**<Level Default*/
    CAM_EV_LEVEL_1       = 1, /**<Level +1 */
    CAM_EV_LEVEL_2       = 3, /**<Level +2 */
    CAM_EV_LEVEL_3       = 5, /**<Level +3 */
} CAM_EV_LEVEL;


typedef enum {
    CAM_STAURATION_LEVEL_MINUS_3 = 6, /**<Level -3 */
    CAM_STAURATION_LEVEL_MINUS_2 = 4, /**<Level -2 */
    CAM_STAURATION_LEVEL_MINUS_1 = 2, /**<Level -1 */
    CAM_STAURATION_LEVEL_DEFAULT = 0, /**<Level Default*/
    CAM_STAURATION_LEVEL_1       = 1, /**<Level +1 */
    CAM_STAURATION_LEVEL_2       = 3, /**<Level +2 */
    CAM_STAURATION_LEVEL_3       = 5, /**<Level +3 */
} CAM_STAURATION_LEVEL;


typedef enum {
    CAM_COLOR_FX_NONE = 0,      /**< no effect   */
    CAM_COLOR_FX_BLUEISH,       /**< cool light   */
    CAM_COLOR_FX_REDISH,        /**< warm   */
    CAM_COLOR_FX_BW,            /**< Black/white   */
    CAM_COLOR_FX_SEPIA,         /**<Sepia   */
    CAM_COLOR_FX_NEGATIVE,      /**<positive/negative inversion  */
    CAM_COLOR_FX_GRASS_GREEN,   /**<Grass green */
    CAM_COLOR_FX_OVER_EXPOSURE, /**<Over exposure*/ //redish
    CAM_COLOR_FX_SOLARIZE,      /**< Solarize   */
} CAM_COLOR_FX;

	
typedef enum {
    CAM_IMAGE_160X120  = 0x00,  /**<160x120 */
    CAM_IMAGE_MODE_320x240   = 0x01,  /**<320x240*/
    CAM_IMAGE_MODE_96X96  = 0x0a,  /**<96x96*/
    CAM_IMAGE_MODE_128X128 = 0x0b, /**<128x128*/
    CAM_IMAGE_MODE_320X320 = 0x0c, /**<320x320*/
    /// @endcond
} CAM_IMAGE_MODE;

/* SSDV */


/* Camera */
// uint8_t arducam_image_size = OV2640_320x240;
// uint8_t arducam_image_format = JPEG;




void arducam_write_register(uint8_t reg, uint8_t value);
uint8_t arducam_read_register(uint8_t reg);
uint32_t arducam_burst_read(uint8_t *buf, uint32_t max_len) ;
void camersSetBrightness(CAM_BRIGHTNESS_LEVEL level ) ;
void cameraSetSharpness(CAM_SHARPNESS_LEVEL level) ;
void cameraSetContrast (CAM_CONTRAST_LEVEL level) ;
void cameraSetEV (CAM_EV_LEVEL level) ;
void cameraSetSaturation (CAM_STAURATION_LEVEL level) ;
void cameraSetColorEffect (CAM_COLOR_FX effect) ;
void arducam_set_jpeg_mode() ;
void arducam_capture_image() ;
uint32_t find_jpeg( uint32_t bytes_read,  uint8_t image_buffer[MAX_IMAGE_SIZE]) ;
void process_image_capture() ;
// void prestore_image();
// void read_prestored_image();

#endif