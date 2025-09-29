#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

#include "model_params.h"
#include "vehicle.h"
#include "crs_converter.h"
#include "gps_messages.h"

struct Args 
{
    std::string input_file;
};

Args parse_args(int argc, char** argv)
{
    if (argc == 2)
    {
        Args args;
        args.input_file = argv[1];
        return args;
    }
    else 
        return Args {""};
}

int main(int argc, char** argv)
{
    Args args = parse_args(argc, argv);
    ModelParams model_params;

    // Грузим параметры модели из текстового файла
    if (!args.input_file.empty())
    {
        std::ifstream file(args.input_file);
        if (!file.is_open())
        {
            std:: cerr << "Невозможно открыть файл " << args.input_file << std::endl;
            return 1;
        }
        load_params_from_file(file, model_params);
    }
    else
    {
        std::cout << "Ожидается имя файла с параметрами модели: gps-tracking-sim <input_file>" << std::endl;
        return -1;
    }

    // Объект Vehicle описывает состояние системы. В конструктор передаём параметры модели.
    Vehicle vehicle(model_params);

    // Азимутальная равноудалённая СК
    const std::string aeqd_crs = "+proj=aeqd +lat_0="
                              + std::to_string(model_params.initial_position[0]) + " +lon_0="
                              + std::to_string(model_params.initial_position[1]) + " +x_0=0 +y_0=0 +datum=WGS84 +units=m";

    CRSConverter cartesian_to_gsk2011(aeqd_crs, "EPSG:7683");

    std::array<double, 2> coords_xy, coords_geo;
    std::string gga_message, vtg_message;

    // Главный цикл
    while (true)
    {
        // Получить текущие декартовы координаты (x, y)
        coords_xy = vehicle.get_position();

        // Переводим в ГСК-2011
        coords_geo = cartesian_to_gsk2011.convert(coords_xy);

        // Генерируем GGA        
        gga_message =generate_gga(std::chrono::system_clock::now(),
                                  coords_geo[0], coords_geo[1],
                                  model_params.initial_elevation);
        // Генерируем VTG
        vtg_message = generate_vtg(vehicle.get_theta(), model_params.speed);   
        
        std::cout << gga_message << vtg_message;

        // Обновить положение системы
        vehicle.update_position(model_params.time_step);

        std::this_thread::sleep_for(std::chrono::milliseconds(model_params.time_step));
    }

    return 0;
}