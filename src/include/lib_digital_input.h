/*****************************************************************************
 * Filename              :   lib_digital_input.h
 * Author                :   Giulio Dalla Vecchia
 * Origin Date           :   23 dic 2021
 *
 *
 * Copyright (c) 2021 Giulio Dalla Vecchia
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 * Author:          Giulio Dalla Vecchia <giulio.dallavecchia@gmail.com>
 * Version:         v0.0.1
 *
 ******************************************************************************/

/** @file lib_digital_input.h
 *  @brief This module handles the doxygen comments.
 * 
 *  This is the header file for the definition of doxygen comments function.
 */

#ifndef LIB_DIGITAL_INPUT_H_
#define LIB_DIGITAL_INPUT_H_

/*****************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * \defgroup        Lib_Digital_Input
 * \brief           Library to manage digital input
 * \{
 */

/*****************************************************************************
 * Module Preprocessor Constants
 ******************************************************************************/

/*****************************************************************************
 * Module Preprocessor Macros
 ******************************************************************************/

/*****************************************************************************
 * Module Typedefs
 ******************************************************************************/

/**
 * @brief  Digital line states
 */
typedef enum {
  DIG_INPUT_ACTIVE_STATE,   /*!< DRV_INPUT_ACTIVED */
  DIG_INPUT_INACTIVE_STATE, /*!< DRV_INPUT_INACTIVED */
  DIG_INPUT_UNKWON_STATE,   /*!< DRV_INPUT_INACTIVED */
  DIG_INPUT_MAX_STATE,
} ldi_state_t;

/** Digital line ID
 */
typedef uint32_t di_id_t;

/**
 * \brief State of the phisical line when it is considered active
 */
typedef enum {
  DIG_INPUT_ACTIVE_LOW,     /**< DIG_INPUT_ACTIVE_LOW */
  DIG_INPUT_ACTIVE_HIGH,    /**< DIG_INPUT_ACTIVE_HIGH */
  DIG_INPUT_MAX_ACTIVE_STATE/**< DIG_INPUT_MAX_ACTIVE_STATE */
} ldi_active_state_t;

/**
 * \brief This structure contains the digital line configuration
 *
 */
typedef struct {
  di_id_t id;
  ldi_active_state_t active_state;
} ldi_cfg_t;

/**
 * \brief Generic digital input line structure
 *
 */
typedef struct di {
  struct di *next;
  di_id_t id;
  ldi_active_state_t active_state;
} ldi_t;

/**
 * \brief This function is used by library to read the phisical state
 *        of a digital line. This function is usually define by user
 *        that call BSP function to read GPIO state.
 */
typedef uint32_t (*ldi_read_state_fn_t)(di_id_t const id);

/*! Fuzione richiamata dalla libreria per segnalare all'utente
 *  che un ingresso digitale a cambiato stato */

/**
 * \brief This function is used by library to signal to the user when
 *        a digital line changes its state
 */
typedef void (*ldi_event_callback_fn_t)(di_id_t const id,
    ldi_state_t const state);

/**
 * \brief Digital input owner configuration structure
 */
typedef struct {
  ldi_read_state_fn_t read_state;
  ldi_event_callback_fn_t event_callback;
} ldi_owner_cfg_t;

/**
 * \brief Digital input owner to manage a maximum of 32 digital lines
 */
typedef struct {

  ldi_read_state_fn_t read_state;
  ldi_event_callback_fn_t event_callback;

  /* Digital input list */
  ldi_t *head_list;

  uint32_t ui32_depressed;
  uint32_t ui32_previous;
} ldi_owner_t;

/*****************************************************************************
 * Module Variable Definitions
 ******************************************************************************/

/*****************************************************************************
 * Function Prototypes
 ******************************************************************************/

void ldi_init(ldi_owner_t *const me, ldi_owner_cfg_t *const cfg_ptr);

void ldi_register(ldi_owner_t *const me, ldi_t *const di_ptr,
    ldi_cfg_t const *const di_cfg_ptr);

ldi_state_t ldi_read_state(ldi_owner_t *const me, di_id_t id);

void ldi_process(ldi_owner_t *const me);

/**
 * \}
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif /*LIB_DIGITAL_INPUT_H_*/

/*** End of File *************************************************************/
