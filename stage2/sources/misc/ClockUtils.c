#include "ClockUtils.h"

uint8_t getUpdateInProgressFlag()
{
    outportb(cmos_address, 0x0A);
    return (inportb(cmos_data) & 0x80);
}

uint8_t getRTCRegister(uint8_t p_reg)
{
    outportb(cmos_address, p_reg);
    return inportb(cmos_data);
}

uint32_t timeStamp32(uint8_t p_seconds, uint8_t p_minutes, uint8_t p_hours, uint8_t p_day, uint8_t p_month, uint16_t p_year)
{
    uint64_t ts = 0;
    int y = 2;
    for (uint16_t i = 1970; i <= p_year; i++)
    {
        if (i == p_year)
        {
            for (uint8_t m = 1; m < p_month; m++)
            {
                if (m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12)
                    ts += (31 * 24 * 60 * 60);
                else if (m == 4 || m == 6 || m == 9 || m == 11)
                    ts += (30 * 24 * 60 * 60);
                else if (m == 2 && y == 0)
                    ts += (29 * 24 * 60 * 60);
                else if (m == 2 && y != 0)
                    ts += (28 * 24 * 60 * 60);
            }
            ts += p_seconds + ((p_minutes - 1) * 60) + ((p_hours - 1) * 60 * 60) + ((p_day - 1) * 24 * 60 * 60);
        }
        else
        {

            if (y == 0)
                ts += (366 * 24 * 60 * 60);
            else
                ts += (365 * 24 * 60 * 60);
        }

        y++;
        y = y % 4;
    }
    return ts - ((8 * 60 * 60) + (55 * 60));
}

uint64_t timeStamp64(uint8_t p_seconds, uint8_t p_minutes, uint8_t p_hours, uint8_t p_day, uint8_t p_month, uint16_t p_year)
{
    return timeStamp32(p_seconds, p_minutes, p_hours, p_day, p_month, p_year);
}

uint32_t getRTCTimeStamp32()
{
    while (getUpdateInProgressFlag())
        ; // Make sure an update isn't in progress
    uint8_t seconds = getRTCRegister(0x00);
    uint8_t minutes = getRTCRegister(0x02);
    uint8_t hours = getRTCRegister(0x04);
    uint8_t day = getRTCRegister(0x07);
    uint8_t month = getRTCRegister(0x08);
    uint16_t year = getRTCRegister(0x09);
    uint8_t registerB = getRTCRegister(0x0B);

    // Convert BCD to binary values if necessary

    if (!(registerB & 0x04))
    {
        seconds = (seconds & 0x0F) + ((seconds / 16) * 10);
        minutes = (minutes & 0x0F) + ((minutes / 16) * 10);
        hours = ((hours & 0x0F) + (((hours & 0x70) / 16) * 10)) | (hours & 0x80);
        day = (day & 0x0F) + ((day / 16) * 10);
        month = (month & 0x0F) + ((month / 16) * 10);
        year = (year & 0x0F) + ((year / 16) * 10);
    }
    if (!(registerB & 0x02) && (hours & 0x80))
    {
        hours = ((hours & 0x7F) + 12) % 24;
    }
    year += 2000;
    return timeStamp32(seconds, minutes, hours, day, month, year);
}

uint64_t getRTCTimeStamp64()
{
    while (getUpdateInProgressFlag())
        ; // Make sure an update isn't in progress
    uint8_t seconds = getRTCRegister(0x00);
    uint8_t minutes = getRTCRegister(0x02);
    uint8_t hours = getRTCRegister(0x04);
    uint8_t day = getRTCRegister(0x07);
    uint8_t month = getRTCRegister(0x08);
    uint16_t year = getRTCRegister(0x09);
    uint8_t registerB = getRTCRegister(0x0B);

    // Convert BCD to binary values if necessary

    if (!(registerB & 0x04))
    {
        seconds = (seconds & 0x0F) + ((seconds / 16) * 10);
        minutes = (minutes & 0x0F) + ((minutes / 16) * 10);
        hours = ((hours & 0x0F) + (((hours & 0x70) / 16) * 10)) | (hours & 0x80);
        day = (day & 0x0F) + ((day / 16) * 10);
        month = (month & 0x0F) + ((month / 16) * 10);
        year = (year & 0x0F) + ((year / 16) * 10);
    }
    if (!(registerB & 0x02) && (hours & 0x80))
    {
        hours = ((hours & 0x7F) + 12) % 24;
    }
    year += 2000;
    uint64_t x = timeStamp64(seconds, minutes, hours, day, month, year);
    /*    if ( p_print_flag)
    {
        Video * video = kernel.getVideo();
        video->setPosition(0,15);
        video->putString("Seconds: ",COLOR_BLUE,COLOR_WHITE);
        video->putDecimal(seconds,COLOR_BLUE,COLOR_WHITE);
        video->putString("\n",COLOR_BLUE,COLOR_WHITE);
        video->putString("minutes: ",COLOR_BLUE,COLOR_WHITE);
        video->putDecimal(minutes,COLOR_BLUE,COLOR_WHITE);
        video->putString("\n",COLOR_BLUE,COLOR_WHITE);
        video->putString("hours: ",COLOR_BLUE,COLOR_WHITE);
        video->putDecimal(hours,COLOR_BLUE,COLOR_WHITE);
        video->putString("\n",COLOR_BLUE,COLOR_WHITE);
        video->putString("day: ",COLOR_BLUE,COLOR_WHITE);
        video->putDecimal(day,COLOR_BLUE,COLOR_WHITE);
        video->putString("\n",COLOR_BLUE,COLOR_WHITE);
        video->putString("month: ",COLOR_BLUE,COLOR_WHITE);
        video->putDecimal(month,COLOR_BLUE,COLOR_WHITE);
        video->putString("\n",COLOR_BLUE,COLOR_WHITE);
        video->putString("year: ",COLOR_BLUE,COLOR_WHITE);
        video->putDecimal(year,COLOR_BLUE,COLOR_WHITE);
        video->putString("\n",COLOR_BLUE,COLOR_WHITE);
        video->putString("stamp: ",COLOR_BLUE,COLOR_WHITE);
        video->putDecimal(x,COLOR_BLUE,COLOR_WHITE);
        video->putString("\n",COLOR_BLUE,COLOR_WHITE);
    }*/
    return x;
}

void RTCSleep(uint16_t seconds)
{
    uint64_t initial_timestamp = getRTCTimeStamp64();
    for (; initial_timestamp + seconds >= getRTCTimeStamp64();)
        ;
}

uint64_t random(uint64_t p_max)
{
    uint64_t next = 100;
    next = next * 1103515245 + 12345 + getRTCTimeStamp32();
    return (unsigned int)(next / 65536) % p_max;
}
