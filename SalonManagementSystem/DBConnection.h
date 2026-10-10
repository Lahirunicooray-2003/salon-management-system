#pragma once
#include <winsock2.h>
#include <mysql.h>
#include <string>
#include "DBConfig.h"

// ENCAPSULATION: connection details private
class DBConnection
{
private:
    MYSQL* conn;

public:
    DBConnection() : conn(nullptr) {}
    ~DBConnection() { Close(); }

    bool Open()
    {
        conn = mysql_init(nullptr);
        if (!conn) return false;

        
        if (!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS,
            DB_NAME, 3306, nullptr, 0))
        {
            Close();
            return false;
        }
        mysql_set_character_set(conn, "utf8");
        return true;
    }

    void Close()
    {
        if (conn) { mysql_close(conn); conn = nullptr; }
    }

    // INSERT / UPDATE / DELETE
    bool Execute(const std::string& sql)
    {
        if (!conn) return false;
        return mysql_query(conn, sql.c_str()) == 0;
    }

    // SELECT
    MYSQL_RES* Query(const std::string& sql)
    {
        if (!conn) return nullptr;
        if (mysql_query(conn, sql.c_str()) != 0) return nullptr;
        return mysql_store_result(conn);
    }

    std::string Escape(const std::string& s)
    {
        std::string out(s.length() * 2 + 1, '\0');
        unsigned long n = mysql_real_escape_string(conn, &out[0], s.c_str(), (unsigned long)s.length());
        out.resize(n);
        return out;
    }
};
