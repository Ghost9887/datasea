table("users") {
	column("id", int(0..), increment),
	column("id2", int(0..), increment),
	column("name", varchar(100), "first_name last_name"),
	column("age", int(1..100)),
	column("active", boolean),
	count(10)
}
