using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using MySqlConnector;

namespace Server
{
    class MySQL
    {
        public static MySqlConnection Setup()
        {
            return new MySqlConnection(String.Format("Server={0};Port=3306;Database={1};Uid={2};Password={3};", "127.0.0.1", "CheatEngine", "root", ""));
        }

        public static bool Connect(MySqlConnection connection)
        {
            try
            {
                connection.Open();
                return true;
            }
            catch (MySqlException exception)
            {
                Console.WriteLine(exception.Message);
                return false;
            }
        }

        public static void Disconnect(MySqlConnection connection)
        {
            try
            {
                connection.Close();
            }
            catch (MySqlException exception)
            {
                Console.WriteLine(exception.Message);
            }
        }

        public static bool GetClientData(string console_key, ref ClientInfo data)
        {
            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    command.CommandText = string.Format("SELECT * FROM users WHERE cpu = @key");
                    command.Parameters.AddWithValue("@key", console_key);
                    using (var reader = command.ExecuteReader())
                    {
                        if (reader.Read())
                        {
                            data.iID = reader.GetInt32("id");
                            data.CPUKey = reader.GetString("cpu");
                            data.FirstIP = reader.GetString("first_ip");
                            data.LastIP = reader.GetString("last_ip");
                            data.Status = (ClientInfoStatus)reader.GetInt32("status");
                            data.iLastConnection = reader.GetInt32("last_connection");
                            Disconnect(db);
                            return true;
                        }
                    }
                }
                Disconnect(db);
            }
            return false;
        }

        public static List<XexInfo> GetXexInfos()
        {
            List<XexInfo> xexInfos = new List<XexInfo>();

            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    command.CommandText = string.Format("SELECT * FROM xex_data");
                    using (var reader = command.ExecuteReader())
                    {
                        if (reader.HasRows)
                        {
                            while (reader.Read())
                            {
                                XexInfo data = new XexInfo
                                {
                                    iID = reader.GetInt32("id"),
                                    Name = reader.GetString("name"),
                                    dwTitle = Convert.ToUInt32(reader.GetString("title"), 16),
                                    dwTitleTimestamp = Convert.ToUInt32(reader.GetString("title_timestamp"), 16),
                                    bEnabled = reader.GetBoolean("enabled"),
                                };

                                xexInfos.Add(data);
                            }
                        }
                    }
                }
                Disconnect(db);
            }

            return xexInfos;
        }

        public static bool GetXexInfo(int id, ref XexInfo data)
        {
            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    command.CommandText = string.Format("SELECT * FROM xex_data WHERE `id` = @key");
                    command.Parameters.AddWithValue("@key", id);
                    using (var reader = command.ExecuteReader())
                    {
                        if (reader.Read())
                        {
                            data.iID = reader.GetInt32("id");
                            data.Name = reader.GetString("name");
                            data.dwTitle = reader.GetUInt32("title");
                            data.dwTitleTimestamp = reader.GetUInt32("title_timestamp");
                            data.bEnabled = reader.GetBoolean("enabled");

                            Disconnect(db);
                            return true;
                        }
                    }
                }
                Disconnect(db);
            }
            return false;
        }

        public static void UpdateUserInfoWelcomePacket(string consolekey, string ip)
        {
            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    command.CommandText = string.Format("UPDATE users SET last_ip = @last_ip, last_connection = @last_connection WHERE `cpu` = @console_key");
                    command.Parameters.AddWithValue("@last_ip", ip);
                    command.Parameters.AddWithValue("@console_key", consolekey);
                    command.Parameters.AddWithValue("@last_connection", Utils.GetTimeStamp());
                    command.ExecuteNonQuery();
                }
                Disconnect(db);
            }
        }

        public static void AddUserWelcomePacket(string consolekey, string cpu, string ip)
        {
            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    command.CommandText = string.Format("INSERT INTO users (cpu, first_ip, last_ip, status) VALUES (@cpu, @first_ip, @last_ip, @status)");
                    command.Parameters.AddWithValue("@time_end", (int)Utils.GetTimeStamp() + 604800);

                    command.Parameters.AddWithValue("@cpu", cpu);
                    command.Parameters.AddWithValue("@first_ip", ip);
                    command.Parameters.AddWithValue("@last_ip", ip);
                    command.Parameters.AddWithValue("@status", ClientInfoStatus.Authed);

                    command.ExecuteNonQuery();
                }
                Disconnect(db);
            }
        }
    }
}
