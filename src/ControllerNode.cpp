#ifndef PROJECTILE_H
#define PROJECTILE_H

// #include <string>

#include <iostream>
#include <fstream>
#include <vector>



class ControllerNode
{
public:
    
    int disk_size = 64;
    int disk_amount = 4;
    const char* disk_names[4] = {"disks/disk_node_1.bin", "disks/disk_node_2.bin", "disks/disk_node_3.bin", "disks/disk_node_4.bin"};

    ControllerNode()
    {
        std::cout << "Controller Node constructor." << std::endl;
    }
    
    // Creates disks if they don't exist and writes something to check.
    // Returns 1 for errors.
    int TestDisks()
    {
        

        for (size_t i = 0; i < disk_amount; i++)
        {
            const char* disk_name = disk_names[i];
            // 1. Create or open file in binary read/write mode
            std::fstream disk(disk_name, std::ios::in | std::ios::out | std::ios::binary);

            // If file doesn't exist, create and set its size to disk_size
            if (!disk)
            {
                std::ofstream createFile(disk_name, std::ios::binary);
                std::vector<char> emptyData(disk_size, 0);  // Initialize with zeroes. a disk_size amount of empty bytes.
                createFile.write(emptyData.data(), emptyData.size());
                createFile.close();

                // Re-open with read/write
                disk.open(disk_name, std::ios::in | std::ios::out | std::ios::binary);
            }

            if (!disk)
            {
                std::cout << "Failed to open file." << std::endl;
                return 1;
            }

            // 2. Write bytes at a specific position (e. g.: position 100)
            disk.seekp(100, std::ios::beg);  // set write pointer
            char dataToWrite[] = {(char)0xDE, (char)0xAD, (char)0xBE, (char)0xEF};
            disk.write(dataToWrite, sizeof(dataToWrite));

            // 3. Read bytes back from the same position
            disk.seekg(100, std::ios::beg);  // set read pointer
            char buffer[4];
            disk.read(buffer, sizeof(buffer));

            std::cout << "Read bytes:";
            for (char b : buffer) {
                printf(" %02X", (unsigned char)b);
            }
            std::cout << std::endl;

            disk.close();
        }

        std::cout << "Exits TestDisks() with 0. Wrote bytes to the disks at 100." << std::endl;
        return 0;
    }

    void DisplayDisks()
    {
        std::cout << "   D1 D2 D3 D4" << std::endl;
        // Finds the disks.
        std::vector<std::fstream> disks(disk_amount);
        for (size_t i = 0; i < disk_amount; i++)
        {
            // Uses emplace_back because std::fstream is non-copyable.
            disks.emplace_back(disk_names[i], std::ios::in | std::ios::binary);
        }
        // For each 1 byte block.
        for (size_t i = 0; i < disk_size; i++)
        {
            std::cout << i << ": ";
            // For each disk.
            for (size_t j = 0; j < disk_amount; j++)
            {
                disks[j].seekg(i, std::ios::beg);
                char byte;
                disks[j].read(&byte, 1);

                printf(" %02X", (unsigned char)byte);
                std::cout << " ";
            }
            std::cout << std::endl;
        }
    }



private:
    


};









#endif