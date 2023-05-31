#include <iostream>
#include <sqlite3.h>

#include "logLib.h"
class LedList
{

public:
    LedList()
    {
        m_db = nullptr;
        int rc = sqlite3_open_v2("example.db", &m_db, SQLITE_OPEN_READONLY, nullptr);
        if (rc != SQLITE_OK)
        {
            SPDLOG_ERROR("Failed to open database: {}" , std::string(sqlite3_errmsg(m_db)));

        }

        if (m_db)
        {
           SPDLOG_DEBUG( "Opened database successfully");
        }
    }
    void query(std::string parkid, std::vector<std::string> &leds)
    {
        try
        {
            _query(parkid, leds);
        }
        catch (const std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    ~LedList()
    {
        if (m_db)
            sqlite3_close(m_db);
        m_db = nullptr;
    }

private:
void createDB()
{
    sqlite3* db;
    int rc = sqlite3_open("example.db", &db);
    if (rc != SQLITE_OK) {
      throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db)));
    }
}
    void _query(std::string parkid, std::vector<std::string> &leds)
    {
        // todo query Leds from leds table which the parkid='parkid'
        const char *query = "SELECT ledid FROM leds WHERE parkid=?";
        sqlite3_stmt *stmt;
        rc = sqlite3_prepare_v2(m_db, query, -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            throw std::runtime_error("Failed to prepare query: " + std::string(sqlite3_errmsg(m_db)));
        }
        rc = sqlite3_bind_text(stmt, 1, parkid.c_str(), -1, SQLITE_TRANSIENT);
        if (rc != SQLITE_OK)
        {
            throw std::runtime_error("Failed to bind parameter: " + std::string(sqlite3_errmsg(m_db)));
        }
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            std::string ledid(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
            std::cout << "LED #" << ledid << std::endl;
            leds.push_back(ledid);
        }
        if (rc != SQLITE_DONE)
        {
            throw std::runtime_error("Failed to execute query: " + std::string(sqlite3_errmsg(m_db)));
        }

        // Close the SQLite database
        sqlite3_finalize(stmt);
    }
    sqlite3 *m_db;
};