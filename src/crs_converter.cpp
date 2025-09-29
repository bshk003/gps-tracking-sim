#include <array>
#include <string>
#include <proj.h>
#include <stdexcept>
#include <iostream>
#include <cmath>

#include "crs_converter.h"

CRSConverter::CRSConverter(const std::string& crs_from, const std::string& crs_to)
{
    m_convert_operation = proj_create_crs_to_crs(PJ_DEFAULT_CTX, crs_from.c_str(), crs_to.c_str(), NULL);
    if (NULL == m_convert_operation)
    {
        std::cerr << "Ошибка при создании преобразователя из " << crs_from << " в " << crs_to << std::endl;
        throw;
    }
}

std::array<double, 2> CRSConverter::convert(std::array<double, 2> coords)
{
    PJ_COORD input_coords, output_coords;

    input_coords = proj_coord(coords[0], coords[1], 0.0, 0.0);
    output_coords = proj_trans(m_convert_operation, PJ_FWD, input_coords);

    if (HUGE_VAL == output_coords.v[3])
    {
        std::cerr << "Ошибка при преобразовании координат" << std::endl;
        throw;
    }

    return {output_coords.xy.x, output_coords.xy.y};
}