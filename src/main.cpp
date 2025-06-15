#include <crow.h>
#include <crow/middlewares/cors.h>
#include <fstream>
#include <sys/stat.h>
#include <chrono>
#include <iomanip>
#include <random>

/// CHANGE THIS INCLUDE TO INCLUDE THE HEADER (.H) AND NOT THE .CPP ///
#include "ControllerNode.cpp"
/// CHANGE THIS INCLUDE TO INCLUDE THE HEADER (.H) AND NOT THE .CPP ///

// Función para verificar si un directorio existe, si no, crearlo
bool ensure_directory_exists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        // Directorio no existe, intentar crearlo
        #ifdef _WIN32
            int status = _mkdir(path.c_str());
        #else
            int status = mkdir(path.c_str(), 0733); // permisos en Unix
        #endif
        return (status == 0);
    } else if (info.st_mode & S_IFDIR) {
        // Directorio ya existe
        return true;
    }
    // Existe pero no es un directorio
    return false;
}

// Función para generar un nombre de archivo único
std::string generate_unique_filename(const std::string& original_name) {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S_");
    
    // Agregar un número aleatorio para mayor unicidad
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    ss << dis(gen) << "_" << original_name;
    
    return ss.str();
}

std::string get_filename_from_headers(const crow::multipart::mph_map& headers) {
    for (const auto& header : headers) {
        if (header.first == "Content-Disposition") {
            const std::string& content_disp = header.second.value;
            size_t filename_pos = content_disp.find("filename=\"");
            if (filename_pos != std::string::npos) {
                filename_pos += 10; // Longitud de "filename=\""
                size_t filename_end = content_disp.find("\"", filename_pos);
                if (filename_end != std::string::npos) {
                    return content_disp.substr(filename_pos, filename_end - filename_pos);
                }
            }
        }
    }
    return "unnamed_file";
}

int main() {

    ControllerNode controller_node;
    controller_node.TestDisks();

    std::cout << "DisplayDisks(0, 64)" << std::endl;
    controller_node.DisplayDisks(0, 64);
    
    // // Test CountCheckDisks(start, end):
    // std::cout << "controller_node.CountCheckBytes(11, 15): " << controller_node.CountCheckBytes(11, 15) << std::endl;
    // std::cout << "controller_node.CountCheckBytes(11, 16): " << controller_node.CountCheckBytes(11, 16) << std::endl;
    // std::cout << "controller_node.CountCheckBytes(11, 17): " << controller_node.CountCheckBytes(11, 17) << std::endl;
    // std::cout << "controller_node.CountCheckBytes(11, 18): " << controller_node.CountCheckBytes(11, 18) << std::endl;
    // std::cout << "controller_node.CountCheckBytes(11, 20): " << controller_node.CountCheckBytes(11, 20) << std::endl;
    // std::cout << "controller_node.CountCheckBytes(11, 21): " << controller_node.CountCheckBytes(11, 21) << std::endl;
    // std::cout << "controller_node.CountCheckBytes(11, 22): " << controller_node.CountCheckBytes(11, 22) << std::endl;
    // std::cout << "controller_node.CountCheckBytes(11, 23): " << controller_node.CountCheckBytes(11, 23) << std::endl;
    // std::cout << "controller_node.CountCheckBytes(11, 24): " << controller_node.CountCheckBytes(11, 24) << std::endl;
    // std::cout << "controller_node.CountCheckBytes(11, 25): " << controller_node.CountCheckBytes(11, 25) << std::endl;
    // std::cout << "controller_node.CountCheckBytes(11, 26): " << controller_node.CountCheckBytes(11, 26) << std::endl;
    // std::cout << "controller_node.CountCheckBytes(11, 27): " << controller_node.CountCheckBytes(11, 27) << std::endl;
    // std::cout << "controller_node.CountCheckBytes(11, 28): " << controller_node.CountCheckBytes(11, 28) << std::endl;
    // std::cout << "controller_node.CountCheckBytes(11, 29): " << controller_node.CountCheckBytes(11, 29) << std::endl;


    // // Test CountDataBytes(start, end):
    // std::cout << "controller_node.CountDataBytes(11, 15): " << controller_node.CountDataBytes(11, 15) << std::endl;
    // std::cout << "controller_node.CountDataBytes(11, 16): " << controller_node.CountDataBytes(11, 16) << std::endl;
    // std::cout << "controller_node.CountDataBytes(11, 17): " << controller_node.CountDataBytes(11, 17) << std::endl;
    // std::cout << "controller_node.CountDataBytes(11, 18): " << controller_node.CountDataBytes(11, 18) << std::endl;
    // std::cout << "controller_node.CountDataBytes(11, 20): " << controller_node.CountDataBytes(11, 20) << std::endl;
    // std::cout << "controller_node.CountDataBytes(11, 21): " << controller_node.CountDataBytes(11, 21) << std::endl;
    // std::cout << "controller_node.CountDataBytes(11, 22): " << controller_node.CountDataBytes(11, 22) << std::endl;
    // std::cout << "controller_node.CountDataBytes(11, 23): " << controller_node.CountDataBytes(11, 23) << std::endl;
    // std::cout << "controller_node.CountDataBytes(11, 24): " << controller_node.CountDataBytes(11, 24) << std::endl;
    // std::cout << "controller_node.CountDataBytes(11, 25): " << controller_node.CountDataBytes(11, 25) << std::endl;
    // std::cout << "controller_node.CountDataBytes(11, 26): " << controller_node.CountDataBytes(11, 26) << std::endl;
    // std::cout << "controller_node.CountDataBytes(11, 27): " << controller_node.CountDataBytes(11, 27) << std::endl;
    // std::cout << "controller_node.CountDataBytes(11, 28): " << controller_node.CountDataBytes(11, 28) << std::endl;
    // std::cout << "controller_node.CountDataBytes(11, 29): " << controller_node.CountDataBytes(11, 29) << std::endl;


    // Test FindStartPosition(size, start):
    // std::cout << "controller_node.FindStartPosition(\"Emmy\" , 64): " << controller_node.FindStartPosition("Emmy" , 64) << std::endl;


    // // Test FindStartPositionTest(size, start):
    // std::cout << "controller_node.FindStartPositionTest(\"Emmy\" , 64): " << controller_node.FindStartPositionTest("Emmy" , 64) << std::endl;
    // std::cout << "controller_node.FindStartPositionTest(\"Franny\" , 128): " << controller_node.FindStartPositionTest("Franny" , 128) << std::endl;
    // std::cout << "controller_node.FindStartPositionTest(\"Molly\" , 256): " << controller_node.FindStartPositionTest("Molly" , 256) << std::endl;
    // std::cout << "controller_node.FindStartPositionTest(\"Molly\" , 256): " << controller_node.FindStartPositionTest("Amy" , 1024) << std::endl;

    // //?
    // // std::cout << "controller_node.CountDataBytes(0, controller_node.ShowFiles()): " << controller_node.CountDataBytes(0, controller_node.ShowFiles()) << std::endl;
    // controller_node.ShowFiles();
    // std::cout << "Total bytes to allocate: " << 64 + 128 + 256 + 1024 << std::endl;
    // std::cout << "controller_node.CountDataBytes(0, 1962): " << controller_node.CountDataBytes(0, 1962) << std::endl;

    // for (size_t i = 0; i < controller_node.files.size(); i++)
    // {
    //     std::cout << "controller_node.CountDataBytes(start_i, end_i): " << controller_node.CountDataBytes(controller_node.files[i].start_position, controller_node.files[i].end_position) << std::endl;
    // }

    // 78
    // 7E
    // 88
    // 17
    // 87
    // E8
    // 81
    // 78
    // 7E
    // 88
    // 17
    // 87
    // E8
    // 81
        
    // Test WriteFile(std::string name, int size, int start, char* data)
    // Emmy
    char emmy_file[16] = {(char)0x78, (char)0x7E, (char)0x88, (char)0x17, (char)0x87, (char)0xE8, (char)0x81, (char)0x78,
        (char)0x7E, (char)0x88, (char)0x17, (char)0x87, (char)0xE8, (char)0x81, (char)0x78, (char)0x7E};

    std::cout << "controller_node.WriteFile(\"Emmy\", 16, 0, emmy_file): " << std::endl;
    controller_node.WriteFile("Emmy", 16, 0, emmy_file);
    controller_node.ShowFiles();
    controller_node.DisplayDisks(0,64);
    
    for (size_t i = 0; i < 5; i++)
    {
        printf("%02X", (unsigned char)(emmy_file[3*i] ^ emmy_file[3*i+1] ^ emmy_file[3*i+2]));
        std::cout << std::endl;
    }
    

    // 64
    // 9B
    // 57
    // 16
    // 49
    // B5
    // 71
    // 64
    // 9B
    // 57
    // 16
    // 49
    // B5
    // 71
    // 64
    // 9B
    // 57
    // 16
    // 49
    // B5
    // 71
    // 64
    // 9B
    // 57
    // 16
    // 49
    // B5
    // 71




    // Tilly
    char tilly_file[28] = {(char)0x64, (char)0x9B, (char)0x57, (char)0x16, (char)0x49, (char)0xB5, (char)0x71, (char)0x64,
        (char)0x9B, (char)0x57, (char)0x16, (char)0x49, (char)0xB5, (char)0x71, (char)0x64, (char)0x9B,
        (char)0x57, (char)0x16, (char)0x49, (char)0xB5, (char)0x71, (char)0x64, (char)0x9B, (char)0x57, (char)0x16, (char)0x49, (char)0xB5, (char)0x71};

    std::cout << "controller_node.WriteFile(\"Tilly\", 16, 0, tilly_file): " << std::endl;
    controller_node.WriteFile("Tilly", 28, 32, tilly_file);
    controller_node.ShowFiles();
    controller_node.DisplayDisks(0,64);
    
    for (size_t i = 0; i < 7; i++)
    {
        printf("%02X", (unsigned char)(tilly_file[3*i] ^ tilly_file[3*i+1] ^ tilly_file[3*i+2]));
        std::cout << std::endl;
    }
    







    // return 0;









    /// Server initialization ///


    // Configurar la aplicación Crow con CORS
    crow::App<crow::CORSHandler> app;
    
    // Configurar CORS
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors
      .global()
        .headers("X-Custom-Header", "Content-Type")
        .methods("POST"_method);
    
    // Ruta para verificar que el servidor está funcionando
    CROW_ROUTE(app, "/")([](){
        return "API de carga de archivos";
    });
    
    // Ruta para subir archivos
    CROW_ROUTE(app, "/upload").methods("POST"_method)([&controller_node](const crow::request& req){
        // Directorio donde se guardarán los archivos
        const std::string upload_dir = "pdfs";
        
        // Verificar y crear el directorio si no existe
        if (!ensure_directory_exists(upload_dir)) {
            return crow::response(500, "No se pudo crear el directorio de subidas");
        }

        std::cout << req.body << std::endl;
        
        // Parsear el cuerpo de la petición multipart
        crow::multipart::message file_message(req);
        
        // Verificar que hay archivos en la petición
        if (file_message.parts.size() == 0) {
            return crow::response(400, "No se encontraron archivos en la petición");
        }
        
        // Procesar cada parte (archivo)
        std::vector<std::string> saved_files;
    

        for (const auto& part : file_message.parts) {
            std::cout << "Parte: " << part.body << std::endl;
        }
        for (const auto& part : file_message.parts) {
            // Obtener el nombre original del archivo
            std::string filename = get_filename_from_headers(part.headers);
            
            // Generar un nombre único para el archivo
            std::string unique_filename = generate_unique_filename(filename);
            std::string filepath = upload_dir + "/" + unique_filename;
            
            /// Saving the file ///
            // This old version saves a file equivalent to the original and with the same name.
            // Guardar el archivo en el sistema
            std::ofstream out_file(filepath, std::ios::binary);
            if (!out_file) {
                return crow::response(500, "Error al crear el archivo en el servidor");
            }
            
            out_file.write(part.body.data(), part.body.size());
            out_file.close();
            
            saved_files.push_back(unique_filename);
            
            // New version saves the file into the disks of the raid and creates a matching file entry in the files vector of ControllerNode.
            // Gets a suitable start position to save the file into.
            int file_size = part.body.size();
            for (size_t i = 0; i < file_size; i++)  // For each byte of the incoming message file.
            {
                int start = controller_node.FindStartPositionTest(filename , file_size);

            }
            
            


            /// Saving the file ///
        }
        
        // Construir respuesta JSON
        crow::json::wvalue response;
        response["status"] = "success";
        response["message"] = "Archivos subidos correctamente";
        response["saved_files"] = crow::json::wvalue::list();
        for (size_t i = 0; i < saved_files.size(); ++i) {
            response["saved_files"][i] = saved_files[i];
        }
        
        return crow::response(response);
    });
        CROW_ROUTE(app, "/get_file").methods("GET"_method)
    ([](){
        return "retornando los files";
    });
    CROW_ROUTE(app, "/get_file/<string>").methods("GET"_method)
    ([](std::string filename){
        return "retornando el file "+ filename;
    });
    CROW_ROUTE(app, "/delete_file/<string>").methods("DELETE"_method)
    ([](std::string filename){
        return "eliminando el file "+ filename;
    });

    // Configurar y ejecutar el servidor
    app.port(18080).multithreaded().run();
    
    return 0;

}