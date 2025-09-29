#include "model_params.h"

// Заполняем словарь <назв_параметра: значение>
void load_raw_params(std::ifstream &file, std::map<std::string, std::string>& params_dict)
{
    std::string line;
    std::string param_name, param_value;

    while (std::getline(file, line))
    {
        size_t col_pos = line.find(':');
        if (col_pos == std::string::npos || line[0] == '#') continue;
        
        param_name = line.substr(0, col_pos);
        param_value = line.substr(col_pos + 1);

        param_name.erase(0, param_name.find_first_not_of(" \t\n\r"));
        param_value.erase(0, param_value.find_first_not_of(" \t\n\r"));

        params_dict[param_name] = param_value;
    }
}

// Конвертируем строки в числовые значения и заполняем ModelParams
void assign_values(std::map<std::string, std::string>& params_dict, ModelParams& model_params)
{
    try
    {
        model_params.steering_wheel_angle = std::stod(params_dict["steering_wheel_angle"]);
        model_params.steering_ratio = std::stod(params_dict["steering_ratio"]);
        model_params.speed = std::stod(params_dict["speed"]);
        model_params.wheelbase = std::stod(params_dict["wheelbase"]);
        model_params.time_step = std::stoi(params_dict["time_step"]);
        model_params.initial_position = { std::stod(params_dict["initial_lat"]),
                                          std::stod(params_dict["initial_long"]) };
        model_params.initial_elevation = std::stod(params_dict["initial_elevation"]);
        model_params.initial_orientation = std::stod(params_dict["initial_orientation"]);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Ошибка при обработке параметров: " << e.what() << '\n';
    }
}

// Грузим параметры модели из файла
// Формат: последовательность строк вида <назв_параметра: значение_параметра>
void load_params_from_file(std::ifstream& file, ModelParams& model_params)
{
    std::map<std::string, std::string> params_dict;

    load_raw_params(file, params_dict);
    assign_values(params_dict, model_params);
}
