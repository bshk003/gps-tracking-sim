#ifndef VEHICLE_STATE_H
#define VEHICLE_STATE_H

#include <cmath>
#include <boost/numeric/odeint.hpp>

#include "model_params.h"

// "Велосипедная" модель. Состояние -- тройка (x, y, theta)
class Vehicle
{
    public:       
        typedef std::array<double, 3> VehicleState;

        Vehicle (const ModelParams& model_params)
            :m_v(model_params.speed * 1000.0 / 3600.0),
             m_L(model_params.wheelbase),
             m_tan_delta(tan(model_params.steering_wheel_angle / model_params.steering_ratio * M_PI / 180.0)),
             m_t(0),
             m_state({0.0, 0.0, model_params.initial_orientation * M_PI / 180.0}) {}

        void operator()(const VehicleState& x, VehicleState& dxdt, double t) const
        {
            // Здесь, x -- обобщённая координата (x, y, theta)
            // Пишем дифур для модели:
            dxdt[0] = m_v * cos(x[2]);
            dxdt[1] = m_v * sin(x[2]);
            dxdt[2] = m_v / m_L * m_tan_delta;
        }

        void update_position(int dt) // dt -- в миллисекундах; внутреннее время m_t -- в секундах.
        {
            rk4_solver.do_step(*this, m_state, m_t, dt/1000.0);
            m_t += dt;
        }

        std::array<double, 2> get_position()
        {
            return {m_state[0], m_state[1]};
        }

        double get_theta()
        {
            return m_state[2];
        }

    private:
        double m_v, m_L, m_t; // всё в СИ
        double m_tan_delta;
        VehicleState m_state;
        
        boost::numeric::odeint::runge_kutta4<VehicleState> rk4_solver;
};


#endif