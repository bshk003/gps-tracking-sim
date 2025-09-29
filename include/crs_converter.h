#ifndef CRS_CONVERTER_H
#define CRS_CONVERTER_H

#include <proj.h>

class CRSConverter
{
    public:
        CRSConverter(const std::string& crs_from, const std::string& crs_to);        
        ~CRSConverter()
        {
            proj_destroy(m_convert_operation);
        }

        std::array<double, 2> convert(std::array<double, 2> coords);

    private:
        PJ* m_convert_operation;
};

#endif