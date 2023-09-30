#include "clocked_interface.h"

template <typename T>
ClockedInterface<T>::ClockedInterface(T *v_module, TbClock *clock)
{
    this->clock = clock;
    this->clock->changed_callback_falling = *(this->clk_assign_falling);
    this->clock->changed_callback_rising = *(this->clk_assign_rising);
    m_core = v_module;
}

template <typename T>
ClockedInterface<T>::~ClockedInterface()
{
}

template <typename T>
void ClockedInterface<T>::eval()
{
}

template <typename T>
void ClockedInterface<T>::clk_assign_rising()
{
}

template <typename T>
void ClockedInterface<T>::clk_assign_falling()
{
}

template <typename T>
void ClockedInterface<T>::tick()
{
}