#ifndef MODEL_PARAMS_H
#define MODEL_PARAMS_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <array>
#include <map>

struct ModelParams 
{
    // Параметры модели
    double steering_wheel_angle; // угол поворота руля (град.)
    double steering_ratio; // передаточное число руль:колесо
    double speed; // скорость (км/ч)
    double wheelbase; // колёсная база (м)
    int time_step; // дискретность (мс)
    std::array<double, 2> initial_position; // начальное положение -- широта, долгота (град.)
    double initial_elevation; // начальное положение -- высота над уровнем моря (м)
    double initial_orientation; // начальное положение -- угол относительно направления на восток (град.)
};

// Заполняем словарь <назв_параметра: значение>
void load_raw_params(std::ifstream &file, std::map<std::string, std::string>& params_dict);

// Конвертируем строки в числовые значения и заполняем ModelParams
void assign_values(std::map<std::string, std::string>& params_dict, ModelParams& model_params);

// Грузим параметры модели из файла
// Формат: последовательность строк вида <назв_параметра: значение_параметра>
void load_params_from_file(std::ifstream& file, ModelParams& model_params);

#endif