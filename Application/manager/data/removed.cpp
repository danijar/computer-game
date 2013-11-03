
/*
	* Register a property type to the manager, providing its structure in database and converter functions.
	* Parameters are a map with future database field names as keys and their type names as value,
	* and functions to read important data from struct to a map and from a map into the struct again.
	* Return type indicates success.
	*/
/*
template <typename T>
bool Register(std::unordered_map<std::string, std::string> Structure, Serialization Serialize, Deserialization Deserialize)
{
	std::string table = Name<T>();
	auto key = std::type_index(typeid(T));

	if(registered.find(key) != registered.end())
	{
		HelperDebug::Fail("data", "property (" + table + ") is already registered");
		return false;
	}

	std::string query = "CREATE TABLE IF NOT EXISTS " + table + " (";
	bool first = true;
	for(auto i : Structure)
	{
		if(first) first = false;
		else query += ", ";
		query += i.first + " " + i.second;
	}
	query += ");";

	// execute query...

	registered.insert(std::make_pair(key, Functions(Serialize, Deserialize)));
	return true;
*/

/*
	*
	*/
/*
template <typename T>
bool Save(uint64_t Id, T *Instance)
{
	std::string table = Name<T>();
	auto key = std::type_index(typeid(T));

	auto i = registered.find(key);
	if(i == registered.end())
	{
		HelperDebug::Fail("data", "no serialization function for (" + table + ")");
		return false;
	}
	row serialized = i->second.Serialize(Instance);

	std::string keys = "id", values = to_string(Id);
	for(auto i : serialized)
		keys += ", " + i.first, values += ", " + i.second;

	std::string query = "INSERT OR REPLACE INTO " + table + " (" + keys + ") VALUES (" + values + ");";
	// execute query,
	// and return false on fail

	// provide example output
	HelperDebug::Pass("data", query);

	return true;
}
*/

/*
	*
	*/
/*
template <typename T>
bool Load(uint64_t id, T *Instance)
{
	std::string table = Name<T>();
	auto key = std::type_index(typeid(T));

	auto i = registered.find(key);
	if(i == registered.end())
	{
		HelperDebug::Fail("data", "no deserialization function for (" + table + ")");
		return false;
	}

	std::string query = "SELECT * FROM " + table + " WHERE id = " + to_string(id) + ";";
	// execute query,
	// and store result in this variable
	row serialized;

	// provide example output and data for testing
	HelperDebug::Pass("data", query);
	for(int i = 1; i <= 4;++i)
		serialized.insert(std::make_pair("key" + std::to_string(i), "value" + std::to_string(i)));

	bool result = i->second.Deserialize(Instance, serialized);
	return result;
}
*/

/*
	* Load all properties of a given type from database and fill their values in newly allocated structs.
	* Parameter is a function that gets executed for each property in database to fetch or create the related property struct.
	* Return type is a map with entity ids as key and newly created instances as value, like what the analogous entity manager function returns.
	*/
/*
template <typename T>
std::unordered_map<uint64_t, T*> Load(std::function<T*(uint64_t)> Walker)
{

}
*/

/*
	* Shortcut for default walker function that always creates a new instance.
	* Useful for loading a dataset for the first time.
	*/
/*
template <typename T>
std::unordered_map<uint64_t, T*> Load()
{
	Load([]{ return new T(); });
}
*/
