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
    
    controller_node.DisplayDisks();












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
    CROW_ROUTE(app, "/upload").methods("POST"_method)([](const crow::request& req){
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
            
            // Guardar el archivo en el sistema
            std::ofstream out_file(filepath, std::ios::binary);
            if (!out_file) {
                return crow::response(500, "Error al crear el archivo en el servidor");
            }
            
            out_file.write(part.body.data(), part.body.size());
            out_file.close();
            
            saved_files.push_back(unique_filename);
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
        CROW_ROUTE(app, "/get file").methods("GET"_method)
    ([](){
        return "reornado el file";
    });

    // Configurar y ejecutar el servidor
    app.port(18080).multithreaded().run();
    
    return 0;

}