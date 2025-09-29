#ifndef GPS_MESSAGES_H
#define GPS_MESSAGES_H

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cctype>
#include <chrono>

// Переводим время в строку вида ччммсс.сс
std::string get_NMEA_time(const std::chrono::system_clock::time_point &time)
{
    auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(time);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time - seconds).count();

    // Время в секундах (отсчёт от POSIX epoch до time) переводим в читаемый формат
    std::time_t total_time = std::chrono::system_clock::to_time_t(seconds);
    std::tm utc_tm = *std::gmtime(&total_time);

    std::ostringstream time_oss;
    // Поля шириной в 2 символа, заполняем нулями, если надо
    time_oss << std::setw(2) << std::setfill('0') << utc_tm.tm_hour
             << std::setw(2) << std::setfill('0') << utc_tm.tm_min
             << std::setw(2) << std::setfill('0') << utc_tm.tm_sec
             << "." << std::setw(2) << std::setfill('0') << (milliseconds / 10);

    return time_oss.str();
}

// Переводим широту в формат DDMM.MMMM
std::string get_NMEA_lat(double lat)
{
    std::ostringstream lat_oss;

    double abs_degs = std::abs(lat);
    int degs = int(abs_degs);
    double mins = (abs_degs - degs) * 60.0;

    // Поле шириной в 2 символа, заполняем нулями, если надо
    lat_oss << std::setw(2) << std::setfill('0') << degs;
    // 4 десятичных знака, ширина -- 7 символов (1 на точку)
    lat_oss << std::fixed << std::setprecision(4) << std::setw(7) << std::setfill('0') << mins;

    return lat_oss.str();
}

// Переводим долготу в формат DDDMM.MMMM
std::string get_NMEA_lon(double lon)
{
    std::ostringstream lat_oss;

    double abs_degs = std::abs(lon);
    int degs = int(abs_degs);
    double mins = (abs_degs - degs) * 60.0;

    // Поле шириной в 3 символа, заполняем нулями, если надо
    lat_oss << std::setw(3) << std::setfill('0') << degs;
    // 4 десятичных знака, ширина -- 7 символов (1 на точку)
    lat_oss << std::fixed << std::setprecision(4) << std::setw(7) << std::setfill('0') << mins;

    return lat_oss.str();
}

std::string get_checksum(const std::string& str)
{
    unsigned char cs = 0;
    std::ostringstream cs_hex;

    // Xor-им всё и переводим в hex
    for (char c : str) cs ^= static_cast<unsigned char>(c);
    cs_hex << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << int(cs);

    return cs_hex.str();
}


// Для GGA сообщения нужно будет заполнить 16 полей. В рамках имитации часть данных берём "с потолка"
std::string generate_gga(const std::chrono::system_clock::time_point &time,
                         double lat, double lon,
                         double alt=0.0,
                         int quality=1,
                         int num_sat=4, // минимально необходимое число спутников
                         double hdop=0.9,
                         double geoid_sep=0.0)
{
    std::ostringstream message;

    // Заголовок
    message << "$GPGGA,";

    // Время
    message << get_NMEA_time(time) << ",";

    // Широта
    char NS_flag = (lat >= 0)? 'N': 'S';
    message << get_NMEA_lat(lat) << "," << NS_flag << ",";

    // Долгота
    char EW_flag = (lon >= 0)? 'E': 'W';
    message << get_NMEA_lon(lon) << "," << EW_flag << ",";
    
    // Доп. данные -- GPS quality indicator и число принятых спутников
    message << quality << "," << num_sat << ",";

    // Данные о высоте. Фикс.ширина, 1 десятичный знак 
    message << std::fixed << std::setprecision(1) << hdop << ",";
    message << std::fixed << std::setprecision(1) << alt << ",M,";
    message << std::fixed << std::setprecision(1) << geoid_sep << ",M,";
    
    // dgps age, station id оставляем пустыми. Стандарт позволяет
    message << ",";

    // Контрольная сумма
    std::string cs_hex = get_checksum(message.str().substr(1));

    message << "*" << cs_hex << "\r\n";

    return message.str();
}

// Для VTG сообщения нужно будет заполнить 11 полей.
std::string generate_vtg(double theta, double speed)
{
    std::ostringstream message;

    // Заголовок
    message << "$GPVTG,";
    
    // Пересчитывам азимут 
    theta = 90 - theta;
    if (theta < 0) theta += 360;
    message << std::fixed << std::setprecision(1) << theta << ",T,";

    // Пропускаем поле для азимута на сев.магнитный полюс. Стандарт позволяет.
    message << ",M,";
    
    // Скорость в узлах
    double speed_knots = speed *  0.539957;
    message << std::fixed << std::setprecision(1) << speed_knots << ",N,";    

    // Скорость в км/ч
    message << std::fixed << std::setprecision(1) << speed << ",K,";

    // Режим -- симуляция
    message << "S";

    // Контрольная сумма
    std::string cs_hex = get_checksum(message.str().substr(1));

    message << "*" << cs_hex << "\r\n";

    return message.str();
}

#endif