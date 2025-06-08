#include "crow.h"
#include <string>
#include <vector>

int main() {
    crow::SimpleApp app;

    // 1. Agregar documento
    CROW_ROUTE(app, "/documents").methods("POST"_method)
    ([](const crow::request& req) {
        auto json = crow::json::load(req.body);
        if (!json || !json.has("name") || !json.has("content")) {
            return crow::response(400, "Formato inválido. Se requieren 'name' y 'content'");
        }

        std::string name = json["name"].s();
        std::string content = json["content"].s();

        // TODO: Implementar lógica para almacenar el documento
        // y dividirlo en bloques para el RAID

        return crow::response(201, "Documento agregado (lógica de almacenamiento no implementada)");
    });

    // 2. Eliminar documento
    CROW_ROUTE(app, "/documents/<string>").methods("DELETE"_method)
    ([](const crow::request& req, std::string name) {
        // TODO: Verificar si el documento existe
        bool document_exists = false; // Cambiar por tu lógica
        
        if (!document_exists) {
            return crow::response(404, "Documento no encontrado");
        }

        // TODO: Implementar lógica para eliminar el documento
        // y todos sus bloques del RAID

        return crow::response(200, "Documento eliminado (lógica de eliminación no implementada)");
    });

    // 3. Buscar documento por nombre
    CROW_ROUTE(app, "/documents/search/<string>")
    ([](std::string query) {
        // TODO: Implementar lógica de búsqueda
        std::vector<std::string> results; // Cambiar por tus resultados reales
        
        crow::json::wvalue response;
        response["results"] = crow::json::wvalue::list();
        
        for (size_t i = 0; i < results.size(); ++i) {
            response["results"][i] = results[i];
        }
        
        return response;
    });

    // 4. Descargar documento (simulando recuperación RAID)
    CROW_ROUTE(app, "/documents/download/<string>")
    ([](std::string name) {
        // TODO: Verificar si el documento existe
        bool document_exists = false; // Cambiar por tu lógica
        
        if (!document_exists) {
            return crow::response(404, "Documento no encontrado");
        }

        // TODO: Implementar lógica para recuperar todos los bloques
        // y unirlos para reconstruir el documento original
        std::string document_content = "Contenido del documento " + name + " (lógica RAID no implementada)";

        // Preparar respuesta para descarga
        crow::response res(document_content);
        res.add_header("Content-Disposition", "attachment; filename=\"" + name + "\"");
        res.add_header("Content-Type", "application/octet-stream");
        return res;
    });

    // Iniciar servidor
    app.port(18080).multithreaded().run();
}