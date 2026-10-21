#include "button.h"

static bool previous = false;
static bool current  = false;

bool read_button()
{
	previous = current;
	current = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);

	if (previous && !current) // the blue button on the board is set to low when pressed
	{
		return true;
	}
	return false;
}
