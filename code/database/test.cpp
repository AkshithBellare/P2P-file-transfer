#include <vector>
#include <string>
#include <iostream>
#include "sqlite3.h"
using namespace std;

sqlite3* db;

using Record = std::vector<std::string>;
using Records = std::vector<Record>;

int select_callback(void *p_data, int num_fields, char **p_fields, char **p_col_names)
{
  Records* records = static_cast<Records*>(p_data);
  try {
    records->emplace_back(p_fields, p_fields + num_fields);
  }
  catch (...) {
    // abort select on failure, don't let exception propogate thru sqlite3 call-stack
    return 1;
  }
  return 0;
}

Records select_stmt(const char* stmt)
{
  Records records;  
  char *errmsg;
  int ret = sqlite3_exec(db, stmt, select_callback, &records, &errmsg);
  if (ret != SQLITE_OK) {
    std::cerr << "Error in select statement " << stmt << "[" << errmsg << "]\n";
  }
  else {
    std::cerr << records.size() << " records returned.\n";
  }

  return records;
}


int main()
{
  if (sqlite3_open("PUBLISHER.db", &db) != SQLITE_OK) {
    std::cerr << "Could not open database.\n";
    return 1;
  }

  Records records = select_stmt("SELECT distinct category FROM files");
  sqlite3_close(db);

  for (auto& record : records) {
    // do something with your records
    cout<<record[0];
  }

  return 0;
}