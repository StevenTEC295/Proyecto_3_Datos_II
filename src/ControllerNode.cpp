#ifndef PROJECTILE_H
#define PROJECTILE_H

// #include <string>

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>  // for std::sort

#include "RAIDFile.cpp"



class ControllerNode
{
public:
    
    int disk_size = 4096;
    int disk_amount = 4;
    const char* disk_names[4] = {"disks/disk_node_1.bin", "disks/disk_node_2.bin", "disks/disk_node_3.bin", "disks/disk_node_4.bin"};

    std::vector<bool> working_disks = {true, true, true, true};
    
    // Stores File objects. Used to manage available memory.
    // This vector has to be ordered by start position.
    std::vector<RAIDFile> files;

    ControllerNode()
    {
        std::cout << "Controller Node constructor." << std::endl;
    }
    
    // Creates disks if they don't exist and writes something to check.
    // Returns 1 for errors.
    // Currently is not writing to the disks.
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




            // // 2. Write bytes at a specific position (e. g.: position 32)
            // disk.seekp(32, std::ios::beg);  // set write pointer
            // char dataToWrite[] = {(char)0xDE, (char)0xAD, (char)0xBE, (char)0xEF};
            // disk.write(dataToWrite, sizeof(dataToWrite));

            // // 3. Read bytes back from the same position
            // disk.seekg(32, std::ios::beg);  // set read pointer
            // char buffer[4];
            // disk.read(buffer, sizeof(buffer));

            // std::cout << "Read bytes:";
            // for (char b : buffer) {
            //     printf(" %02X", (unsigned char)b);
            // }
            // std::cout << std::endl;




            disk.close();
        }

        std::cout << "Exits TestDisks() with 0. Wrote bytes to the disks at 32." << std::endl;
        return 0;
    }

    void DisplayDisks(int start_block, int end_block)
    {
        std::cout << "   D1  D2  D3  D4" << std::endl;
        // Finds the disks.
        std::vector<std::fstream> disks;
        for (size_t i = 0; i < disk_amount; i++)
        {
            // Uses emplace_back because std::fstream is non-copyable.
            disks.emplace_back(disk_names[i], std::ios::in | std::ios::binary);
        }
        // For each 1 byte block.
        for (size_t i = start_block; i < end_block; i++)
        {
            std::cout << i << ": ";
            // For each disk.
            for (size_t j = 0; j < disk_amount; j++)
            {
                disks[j].seekg(i, std::ios::beg);
                char byte;
                disks[j].read(&byte, 1);

                printf("%02X", (unsigned char)byte);


                std::cout << "  ";
            }
            std::cout << std::endl;
        }
    }

    void DisplayDisks()
    {
        DisplayDisks(0, disk_size);
    }

    // Counts how many Bytes are used for parity information between two positions. [start, end[ 
    int CountCheckBytes(int start, int end)
    {
        int start_block = start/4;
        int end_block = end/4;
        int blocks = end_block - start_block;

        // std::cout << "start_block, " << start_block << "; end_block, " << end_block << "; blocks, " << blocks;

        int C_count = blocks + 1;
        if (!(start%4 <= start_block%4))
        {
            C_count -= 1;
            // std::cout << "; \tsubstract start. ";
        }
        if (!(end%4 > end_block%4))
        {
            C_count -= 1;
            // std::cout << "substract end.";
        }
        // std::cout << " -> ";
        
        /*
         C,  1,  2,  3
         4,  C,  6,  7
         8,  9,  C, 11
        12, 13, 14,  C
         C, 17, 18, 19
        20,  C, 22, 23
        24, 25,  C, 27
        28, 29, 30,  C
        ...
        */

        return C_count;
    }

    int CountDataBytes(int start, int end)
    {
        
        int start_block = start/4;
        int end_block = end/4;
        int blocks = end_block - start_block;

        // std::cout << "start_block, " << start_block << "; end_block, " << end_block << "; blocks, " << blocks;

        int byte_count = blocks*disk_amount;  // Assumes all disks are used for data and then substracs all the check bytes.
        byte_count -= start%4;
        // std::cout << "; start%4, " << start%4;
        // std::cout << "; end%4, " << end%4;
        byte_count += end%4;

        byte_count -= CountCheckBytes(start, end);
        return byte_count;
        

        
    }

    // Returns a start position to write a file given the name and size.
    // Should not add to files vector.
    // Returns -1 if the file doesn't fit in memory or it already exists in the files vector.
    // Actual write logic happens when receiving a file from the client.
    int FindStartPosition(std::string name, int size)
    {
        // Special case when memory is empty.
        if (files.size() == 0)
        {
            std::cout << "Disks are empty";
            if (disk_size*disk_amount*3/4 >= size)
            {
                std::cout << " and file fits in memory." << std::endl;
                return 0;
            }
            else
            {
                std::cout << " but file does not fit in memory." << std::endl;
                return -1;
            }
        }
        
        
        for (size_t i = 0; i < files.size(); i++)
        {
            if (files[i].name == name)
            {
                std::cout << "The file already exists." << std::endl;
                return -1;
            }
        }

        return FindAllocableSpace(size, 0);

        

    }



    // Creates a File object with name and coordinates that fit the size.
    // Adds files as test behaviour.
    int FindStartPositionTest(std::string name, int size)
    {
        int result = -1;
        // Special case when memory is empty.
        if (files.size() == 0)
        {
            std::cout << "Disks are empty";
            if (disk_size*disk_amount*3/4 >= size)
            {
                std::cout << " and file fits in memory." << std::endl;
                result = 0;
            }
            else
            {
                std::cout << " but file does not fit in memory." << std::endl;
                result = -1;
            }
        }
        std::cout << "FindStartPositionTest CP 1" << std::endl;
        
        
        for (size_t i = 0; i < files.size(); i++)
        {
            if (files[i].name == name)
            {
                std::cout << "The file already exists." << std::endl;
                result = -1;
            }
        }
        std::cout << "FindStartPositionTest CP 2" << std::endl;


        if (result != 0)
        {
            result = FindAllocableSpace(size, 0);
        }
        
        std::cout << "FindStartPositionTest CP 3" << std::endl;
        WriteFileTest(name, size, result);
        std::cout << "FindStartPositionTest CP 4" << std::endl;
        return result;

    }







    // Gets start and end positions and checks if there's enough space. If not, jumps to the next possible start position. Recursive with an aux? 

    // Finds the first start position in memory that can fit size.
    // Fails if files vector is empty.
    int FindAllocableSpace(int size, int start)
    {
        // Find nearest occupied byte after start.
        int closest_allocated_byte = files[0].start_position;
        int closest_allocated_byte_index = 0;
        for (size_t i = 1; i < files.size(); i++)
        {
            if (closest_allocated_byte > start)
            {
                break;
            }
            closest_allocated_byte = files[i].start_position;
            closest_allocated_byte_index = i;
        }

        std::cout << "FindAllocableSpace CP 1" << std::endl;

        // If no allocated bytes were found in fron of start (i. e. limit is the end of the disks).
        if (start > closest_allocated_byte)
        {
            if (CountDataBytes(start, disk_size) >= size)
            {
                std::cout << "From FindAllocableSpace() an available block to store (" << size << ") bytes was found at memory position (" << start << ")."; 
                std::cout << " Found block is last in memory." << std::endl;
                return start;
            }
            else
            {
                std::cout << "Failed to find suitable block in memory." << std::endl;
                return -1;
            }
            
            
        }
        
        std::cout << "FindAllocableSpace CP 2" << std::endl;

        // If the space between my current position (start) and the first already allocated byte can fit the file.
        if (CountDataBytes(start, closest_allocated_byte) >= size)
        {
            std::cout << "From FindAllocableSpace() an available block to store (" << size << ") bytes was found at memory position (" << start << ")."; 
            std::cout << " Found block is between other blocks in memory." << std::endl;
            return start;
        }
        else
        {
            std::cout << "FindAllocableSpace CP 3" << std::endl;

            std::cout << "Show files" << std::endl;
            for (size_t i = 0; i < files.size(); i++)
            {
                
                std::cout << "files[i]: (" << files[i].name << ", " << files[i].start_position << ", " << files[i].end_position << ")" << std::endl;
            }
            



            std::cout << "Reenters FindAllocableSpace with start = files[" << closest_allocated_byte_index << "].end_position: " << files[closest_allocated_byte_index].end_position << std::endl;
            return FindAllocableSpace(size, files[closest_allocated_byte_index].end_position);
        }
    }

    // Test funciton to add RAIDFile objects to the files vector.
    // Actual writing is done when receiving a file from the client.
    void WriteFileTest(std::string name, int size, int start)
    {
        int end = start;
        while (size > 0)
        {
            if (!ByteIsParity(end))
            {
                size --;
            }
            else
            {
                //std::cout << "Skipped parity byte." << "start: " << start << ", " << "end: " << end << ", " << "size: " << size << ". " << std::endl;
            }
            
            end ++;
        }
        RAIDFile file_to_add(name, start, end);
        files.push_back(file_to_add);
        std::sort(files.begin(), files.end(), [](const RAIDFile& a, const RAIDFile& b) {
            return a.start_position < b.start_position;
        });

        std::cout << "WriteFileTest(std::string name, int size, int start) writes: (" << name << ", " << start << ", " << end << ")" << std::endl;

    }








    // // Writes a file to the disks and calculates parity bytes.
    // // Writes per block.
    // // Adds a RAIDFile object to the files vector.
    // void WriteFile(std::string name, int size, int start, char* data)
    // {
    //     // end is the current position in memory.
    //     int end = start;


    //     while (size > 0)
    //     {    
    //         // Calculates a std::vector<char> with the data bytes of the current block.


    //         // Number of bytes needed for the current block.
    //         int start_of_next_block = (end+4) - (end)%4;
    //         int number_of_bytes_for_current_block = CountDataBytes(end, start_of_next_block);
    //         if (number_of_bytes_for_current_block > size)
    //         {
    //             // number_of_bytes_for_current_block can't ask for more
    //             number_of_bytes_for_current_block = size;
    //         }
            

    //         std::vector<char> current_block_bytes;
    //         for (size_t i = 0; i < disk_amount; i++)
    //         {
    //             if (!ByteIsParity(end + i))
    //             {
    //                 current_block_bytes.push_back(data[])
    //             }
    //         }
        
        
    //         // and the parity byte/////////////       
        
    //     }

    //     /////////////////////////////////////////////////
    //     while (size > 0)
    //     {
    //         if (!ByteIsParity(end))
    //         {
    //             size --;
    //         }
    //         else
    //         {
    //             std::cout << "Skipped parity byte." << "start: " << start << ", " << "end: " << end << ", " << "size: " << size << ". " << std::endl;
    //         }
            
    //         end ++;
    //     }
    //     RAIDFile file_to_add(name, start, end);
    //     files.push_back(file_to_add);
    //     std::sort(files.begin(), files.end(), [](const RAIDFile& a, const RAIDFile& b) {
    //         return a.start_position < b.start_position;
    //     });

    //     std::cout << "WriteFileTest(std::string name, int size, int start) writes: (" << name << ", " << start << ", " << end << ")" << std::endl;

    // }

    // void CalculateParityByte(int start)
    // {
    //     // moves start position to the beginning of the block.
    //     start -= start%4;
    //     char result = 0x00;
        
    //     std::vector<char> disk_bytes;
    //     for (size_t i = 0; i < disk_amount; i++)
    //     {
    //         if (!ByteIsParity(start + i))
    //         {
    //             disk_bytes.push_back()
    //         }
            
    //     }
        
        
    // }

    void WriteFile(std::string name, int size, int start, const char* data)
    {
        // end is the current position in memory.
        int end = start;
        // current_data_byte index of the bytes in data.
        int current_data_byte = 0;
        // Calculated an iteration after writing the last byte.
        int RAID_file_end = -1;

        // Old writing logic.
        /////////////////////////////////////////////////
        // while (current_data_byte < size)
        // {    
        //     // Calculates a std::vector<char> with the data bytes of the current block.


        //     // Number of bytes needed for the current block.
        //     int start_of_next_block = (end+4) - (end)%4;
        //     int number_of_bytes_for_current_block = CountDataBytes(end, start_of_next_block);
        //     if (number_of_bytes_for_current_block > size)
        //     {
        //         // number_of_bytes_for_current_block can't ask for more
        //         number_of_bytes_for_current_block = size;
        //     }
            

        //     std::vector<char> current_block_bytes;
        //     for (size_t i = 0; i < disk_amount; i++)
        //     {
        //         if (!ByteIsParity(end + i))
        //         {
        //             // current_block_bytes.push_back(data[])
        //         }
        //     }
        
        
        //     // and the parity byte/////////////       
        
        // }

        /////////////////////////////////////////////////
        // Gets the disks.
        std::vector<std::fstream> disks;
        for (size_t i = 0; i < disk_amount; i++)
        {
            // Uses emplace_back because std::fstream is non-copyable.
            disks.emplace_back(disk_names[i], std::ios::in | std::ios::out | std::ios::binary);
        }

        // Variables to trigger parity calculation.
        int current_block = end/4;
        bool current_block_parity_is_calculated = true;
        while (current_data_byte <= size)
        {
            // Write previous block's parity (last block's parity is written individually after while loop).
            // if parity has to be updated and is moving to the next block.
            if ((!current_block_parity_is_calculated && current_block != end/4) || current_data_byte == size)
            {
                // Special case for last written bit (which was written the last iteration).
                if (current_data_byte == size)
                {
                    // Store the end in memory of the file for creating the RAID_file object.
                    RAID_file_end = end;
                    // make end jump to the next block to use the same logic.
                    end = end - end%4 + 4;
                }
                
                // Block's data bytes.
                std::vector<char> current_block_bytes;
                // Get all the data bytes of the previous block.
                for (size_t i = 0; i < disk_amount; i++)
                {
                    if (!ByteIsParity(end - 4 + i))
                    {
                        disks[i].seekg(current_block, std::ios::beg);  // set read pointer.
                        char buffer[1];
                        disks[i].read(buffer, sizeof(buffer));
                        current_block_bytes.push_back(buffer[0]);
                    }
                }
                // Find current_parity_disk as previous block %4.
                int current_parity_disk = (end/4 -1)%4;
                // At previous block
                disks[current_parity_disk].seekp(end/4 -1, std::ios::beg);
                // write.
                char parity_byte_value = CalculateParityByte(current_block_bytes);
                disks[current_parity_disk].write(&(parity_byte_value), sizeof(char));

                current_block_parity_is_calculated = true;
                current_block = end/4;

                if (current_data_byte == size)
                {
                    break;
                }
            }
            

            if (!ByteIsParity(end))
            {
                disks[end%4].seekp(end/4, std::ios::beg);
                disks[end%4].write(&(data[current_data_byte]) , sizeof(char));
                // size --;
                current_data_byte ++;
                current_block_parity_is_calculated = false;
            }
            else
            {
                //std::cout << "Skipped parity byte." << "start: " << start << ", " << "end: " << end << ", " << "size: " << size << ", " << "current_data_byte: " << current_data_byte << ". " << std::endl;
            }
            
            end ++;
        }

        // Save a corresponding RAIDFile object to the files vector.
        RAIDFile file_to_add(name, start, RAID_file_end);
        files.push_back(file_to_add);
        // Sorts the files vector by start_position (necessary for allocating memory for new files).
        std::sort(files.begin(), files.end(), [](const RAIDFile& a, const RAIDFile& b) {
            return a.start_position < b.start_position;
        });

        std::cout << "WriteFile(std::string name, int size, int start) writes: (" << name << ", " << start << ", " << RAID_file_end << ")" << std::endl;

        // Close the disks.
        for (size_t i = 0; i < disks.size(); i++)
        {
            disks[i].close();
        }
        

    }












    // Receives the non parity bytes of the current block and returns a parity byte using XOR on each. 
    char CalculateParityByte(std::vector<char> bytes)
    {
        char parity_byte = 0x00;
        for (size_t i = 0; i < bytes.size(); i++)
        {
            parity_byte ^= bytes[i];
        }
        
        return parity_byte;
    }


    
    bool ByteIsParity(int byte)
    {
        //std::cout << "if ((byte/4)%4 == byte%4) -> " << (byte/4)%4 << " == " << byte%4 << " -> " << ((byte/4)%4 == byte%4) << std::endl;
        if ((byte/4)%4 == byte%4)
        {
            return true;
        }
        return false;
    }


    // Prints the files vector.
    // Returns the total of bytes used in memory.
    int ShowFiles()
    {
        int total_bytes = 0;
        for (size_t i = 0; i < files.size(); i++)
        {   
            std::cout << "files[i]: (" << files[i].name << ", " << files[i].start_position << ", " << files[i].end_position << ")" << std::endl;
            total_bytes += files[i].end_position - files[i].start_position;
        }
        return total_bytes;
    }


    // Writes 0 in all of a disk's fields (replacement disk) and turns the disk's corresponding flag in the working_disks vector to false.
    void SimulateDiskFailure(int disk_id)
    {
        const char* disk_name = disk_names[disk_id];
        // 1. Create or open file in binary read/write mode
        std::fstream disk(disk_name, std::ios::in | std::ios::binary);

        std::vector<char> emptyData(disk_size, 0);  // A disk_size amount of empty bytes.
        
        disk.seekp(0, std::ios::beg);  // set write pointer
        disk.write(emptyData.data(), emptyData.size());

        disk.close();

        working_disks[disk_id] = false;
    }

    // Repairs the disk that shows as false in the working_disks vector.
    // If multiple disks are damaged, the code runs for each but the data is lost.
    void RepairDamagedDisk()
    {
        // Gets the disks.
        std::vector<std::fstream> disks;
        for (size_t i = 0; i < disk_amount; i++)
        {
            // Uses emplace_back because std::fstream is non-copyable.
            disks.emplace_back(disk_names[i], std::ios::in | std::ios::out | std::ios::binary);
        }
        for (size_t disk = 0; disk < disk_amount; disk++)
        {
            if (!working_disks[disk])
            {
                /* CODE TO REPAIR THE DISKS USING XOR. */
                

                for (size_t block = 0; block < disk_size; block++)
                {
                    std::vector<char> current_block_bytes;
                    // Get all the data bytes of the current block.
                    for (size_t current_disk = 0; current_disk < disk_amount; current_disk++)
                    {
                        if (working_disks[current_disk])
                        {
                            disks[current_disk].seekg(block, std::ios::beg);  // set read pointer.
                            char buffer[1];
                            disks[current_disk].read(buffer, sizeof(buffer));
                            current_block_bytes.push_back(buffer[0]);
                        }
                    }

                    char restored_byte = 0x00;
                    for (size_t current_disk = 0; current_disk < current_block_bytes.size(); current_disk++)
                    {
                        restored_byte ^= current_block_bytes[current_disk];
                    }

                    disks[disk].seekp(block, std::ios::beg);
                    disks[disk].write(&(restored_byte) , sizeof(char));

        
                }
            }
        }
    }


    // Returns a std::vector<char> of the bytes in the file.
    std::string GetFile(std::string file_name)
    {
        std::cout << "Enters GetFile() with file_name: " << file_name << std::endl;
        // bool file_found = false;
        int file_index = -1;
        for (size_t i = 0; i < files.size(); i++)
        {
            if (file_name == files[i].name)
            {
                file_index = i;
                break;
            }
        }
        if (file_index == -1)
        {
            std::cout << "GetFile: File not found." << std::endl;
            std::string no_result;
            return no_result;
        }
        
        std::cout << "GetFile: File found at index: " << file_index << std::endl;
        ShowFiles();

        std::vector<char> result;
        int start = files[file_index].start_position;
        int end = files[file_index].end_position;

        // Gets the disks.
        std::vector<std::fstream> disks;
        for (size_t i = 0; i < disk_amount; i++)
        {
            // Uses emplace_back because std::fstream is non-copyable.
            disks.emplace_back(disk_names[i], std::ios::in | std::ios::binary);
        }
        for (size_t i = start; i < end; i++)
        {
            if (!ByteIsParity(i))
            {    
                disks[i%4].seekg(i/4, std::ios::beg);  // set read pointer.
                char buffer[1];
                disks[i%4].read(buffer, sizeof(buffer));
                printf("%02X", (unsigned char)buffer[0]);
                result.push_back(buffer[0]);            
            }
            
        }
        for (size_t i = 0; i < disk_amount; i++)
        {
            disks[i].close();
        }
        std::cout << "result size: " << result.size() << std::endl;

        std::string str(result.begin(), result.end());
        std::cout << "str size: " << str.size() << std::endl;
        return str;
    }

    bool DeleteFile(std::string file_name)
    {
        int file_index = -1;
        for (size_t i = 0; i < files.size(); i++)
        {
            if (file_name == files[i].name)
            {
                file_index = i;
                break;
            }
        }
        if (file_index == -1)
        {
            std::cout << "DeleteFile: File not found." << std::endl;
            return false;
        }
        files.erase(files.begin() + file_index);
        std::cout << "Deleted file: " << file_name << std::endl;
        return true;
    }














private:
    


};









#endif