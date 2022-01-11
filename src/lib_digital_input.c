/*****************************************************************************
 * Filename              :   lib_digital_input.c
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

/** @file lib_digital_input.c
 *  @brief This is the source file for doxygen comments function
 */

/*****************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>				/* For portable types */
#include <assert.h>
#include <stddef.h>
#include "lib_digital_input.h"

/*****************************************************************************
 * Module Preprocessor Constants
 ******************************************************************************/

/*****************************************************************************
 * Module Preprocessor Macros
 ******************************************************************************/

/*****************************************************************************
 * Module Typedefs
 ******************************************************************************/

/*****************************************************************************
 * Function Prototypes
 ******************************************************************************/

/*****************************************************************************
 * Module Variable Definitions
 ******************************************************************************/

/*****************************************************************************
 * Function Definitions
 ******************************************************************************/

/**
 * \brief This function initialize digital line owner.
 *
 * \param me: pointer to the digital line owner
 * \param cfg_ptr: pointer to the digital lines owner configuration
 */
void ldi_init(ldi_owner_t *const me, ldi_owner_cfg_t *const cfg_ptr) {

  assert(me != NULL);
  assert(cfg_ptr != NULL);

  /* Salvo i puntatori a funzioni gestite dall'utente */
  me->read_state = cfg_ptr->read_state;
  me->event_callback = cfg_ptr->event_callback;

  me->ui32_depressed = 0U;
  me->ui32_previous = 0U;

  me->head_list = NULL;
}

/**
 * \brief This function register a new digital line to a owner
 *
 * \param me: pointer to the owner that will manage the digital line
 * \param di_ptr: pointer to the new digital line
 * \param di_cfg_ptr: pointer to the digital line configuration
 */
void ldi_register(ldi_owner_t *const me, ldi_t *const di_ptr,
    ldi_cfg_t const *const di_cfg_ptr) {

  assert(me != NULL);
  assert(di_ptr != NULL);
  assert(di_cfg_ptr != NULL);

  di_ptr->next = me->head_list;
  me->head_list = di_ptr;

  di_ptr->id = di_cfg_ptr->id;
  di_ptr->active_state = di_cfg_ptr->active_state;
}

/**
 * \brief This function process all the digital line that are
 *        managed by a specific owner
 *
 * \param me: pointer to the digital line owner
 */
void ldi_process(ldi_owner_t *const me) {

  uint32_t state;
  uint32_t current = 0U;
  uint32_t tmp;
  uint32_t mask = 1U;
  ldi_t * di;
  uint32_t i = 0U;

  assert(me->read_state != NULL);

  /* Leggo lo stato delle linee */
  for (di = me->head_list; di != (ldi_t*)0; di = di->next) {
    if(me->read_state != NULL) {
      state = (*me->read_state)(di->id);
    }

    if(DIG_INPUT_ACTIVE_LOW == di->active_state) {
      state = !state;
    }
    current = (current & ~(1UL << i)) | (state << i);
    ++i;
  }

  tmp = me->ui32_depressed; /* save the debounced depressed buttons */
  me->ui32_depressed |= (me->ui32_previous & current); /* set depressed */
  me->ui32_depressed &= (me->ui32_previous | current); /* clear released */
  me->ui32_previous = current; /* update the history */
  tmp ^= me->ui32_depressed; /* changed debounced depressed */

  for (di = me->head_list; di != (ldi_t*)0; di = di->next) {
    if((tmp & mask) != 0U) { /* debounced button state changed? */
      if((me->ui32_depressed & mask) != 0U) { /* is button depressed? */
        if(me->event_callback != NULL) {
          (*me->event_callback)(di->id, DIG_INPUT_ACTIVE_STATE);
        }
      } else {
        if(me->event_callback != NULL) {
          (*me->event_callback)(di->id, DIG_INPUT_INACTIVE_STATE);
        }
      }
    } else {
      /* Lo stato della linea non è cambiato */
    }

    mask = mask << 1U; /* Shif mask by one */
  }
}

/*************** END OF FUNCTIONS ********************************************/
