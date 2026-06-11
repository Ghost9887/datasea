table("users") {
	column("id", int(0..)),
	column("name", varchar(100), "first_name last_name"),
	column("age", int(1..100)),
	column("active", boolean),
	count(1000)
}
