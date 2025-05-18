#include "sd_card_manager.h"
#include "fatfs.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "task2.h" // Để lấy GROUP_NUMBER

// Các biến toàn cục
static FATFS sdFatFs; // File system object
static uint8_t sd_initialized = 0;

/**
 * @brief  Khởi tạo SD Card và file system
 * @retval uint8_t: 1 nếu thành công, 0 nếu thất bại
 */
uint8_t SD_Init(void)
{
    FRESULT fresult;

    // Nếu đã khởi tạo rồi thì không khởi tạo lại
    if (sd_initialized)
    {
        return 1;
    }

    // Khởi tạo SD Card qua giao diện SDIO
    if (BSP_SD_Init() != MSD_OK)
    {
        return 0;
    }

    // Mount file system
    fresult = f_mount(&sdFatFs, "", 1);
    if (fresult != FR_OK)
    {
        return 0;
    }

    sd_initialized = 1;
    return 1;
}

/**
 * @brief  Tạo và ghi file với nội dung cho trước
 * @param  filename: Tên file cần tạo/ghi
 * @param  content: Nội dung cần ghi vào file
 * @param  append: 1 = thêm vào cuối file, 0 = ghi đè file
 * @retval uint8_t: 1 nếu thành công, 0 nếu thất bại
 */
uint8_t SD_WriteFile(const char *filename, const char *content, uint8_t append)
{
    FIL file;
    FRESULT fresult;
    UINT bytesWritten;
    BYTE mode;

    // Kiểm tra SD đã khởi tạo chưa
    if (!sd_initialized)
    {
        if (!SD_Init())
        {
            return 0;
        }
    }

    // Xác định chế độ mở file
    mode = append ? (FA_OPEN_APPEND | FA_WRITE) : (FA_CREATE_ALWAYS | FA_WRITE);

    // Mở file để ghi
    fresult = f_open(&file, filename, mode);
    if (fresult != FR_OK)
    {
        return 0;
    }

    // Ghi nội dung vào file
    fresult = f_write(&file, content, strlen(content), &bytesWritten);

    // Đóng file
    f_close(&file);

    return (fresult == FR_OK && bytesWritten == strlen(content)) ? 1 : 0;
}

/**
 * @brief  Tạo file team9.txt với nội dung về nhóm
 * @retval uint8_t: 1 nếu thành công, 0 nếu thất bại
 */
uint8_t SD_CreateTeamFile(void)
{
    char content[50];
    sprintf(content, "Nhom: %d", GROUP_NUMBER);
    return SD_WriteFile(FILE_NAME, content, 0);
}

/**
 * @brief  Đọc nội dung từ file
 * @param  filename: Tên file cần đọc
 * @param  buffer: Buffer để lưu dữ liệu đọc được
 * @param  buffer_size: Kích thước của buffer
 * @retval int: Số byte đọc được, -1 nếu có lỗi
 */
int SD_ReadFile(const char *filename, char *buffer, uint32_t buffer_size)
{
    FIL file;
    FRESULT fresult;
    UINT bytesRead = 0;

    // Kiểm tra SD đã khởi tạo chưa
    if (!sd_initialized)
    {
        if (!SD_Init())
        {
            return -1;
        }
    }

    // Mở file để đọc
    fresult = f_open(&file, filename, FA_READ);
    if (fresult != FR_OK)
    {
        return -1;
    }

    // Xác định kích thước cần đọc
    UINT size_to_read = f_size(&file);
    if (size_to_read >= buffer_size)
    {
        size_to_read = buffer_size - 1;
    }

    // Đọc dữ liệu từ file
    fresult = f_read(&file, buffer, size_to_read, &bytesRead);

    // Đóng file
    f_close(&file);

    if (fresult != FR_OK)
    {
        return -1;
    }

    // Thêm null terminator nếu đọc chuỗi
    buffer[bytesRead] = '\0';

    return bytesRead;
}

/**
 * @brief  Đọc file team9.txt và lưu vào buffer được cung cấp
 * @param  buffer: Buffer để lưu dữ liệu đọc được
 * @param  buffer_size: Kích thước buffer
 * @retval int: Số byte đọc được, -1 nếu có lỗi
 */
int SD_ReadTeamFile(char *buffer, uint32_t buffer_size)
{
    return SD_ReadFile(FILE_NAME, buffer, buffer_size);
}