# Datasea ![License](https://img.shields.io/badge/license-MIT-blue)

[STILL A WORK IN PROGRESS]

Datasea is a language to help you generate SQL data the way you want.

## USAGE
- datasea -h (for help)
- datasea <file_path> [custom_output_name]

## Features
- Fast and lightweight
- Expressive
- Easy syntax
- Programming style generation
- Random or weighted generation of common types with parameters

## Installation
```bash
git clone git@github.com:Ghost9887/datasea.git
cd datasea
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build
```

### EXAMPLE
```
//types available to use
let string = "Hello, World!";
let int = 5;
let double = 3.14;
let bool = true;
let list = ["hello", true, 10, ["nested list"]];

//folder used when generating fake data (first_names, last_names, city, ...)
locale("us");

//table name, amount of rows 
table("users", 10) {
    
    //creating a variable first_name that holds a generated value
    let first_name = gen(_firstname);

    //prints to console
    print(first_name);

    //creating a variable last_name that holds a generated value
    let last_name = gen(_lastname);

    //increment goes from the given start to INT_MAX or if limited to that limit and then restarts (in this case goes back to 0)
    column("id", increment(0..));

    //accessing variables
    column("first_name", first_name);
    column("last_name", last_name);

    //you can format a string into whatever you like using the '{}' as placeholders for values
    column("full_name", format("{} {}", first_name, last_name));

    //you can mix different types in format
    column("username", format("{}{}", first_name, randint(100..999))); 

    //string and lists have a .at() function to index into them
    column("email", format("{}.{}@email.com", first_name.at(0).lower(), last_name.lower()));

    //string also has a substr function which return a substring within that range.
    column("nickname", first_name.substr(0..3));

    //random(start..end) generates a random integer within the range. If not limited goes up to INT_MAX
    column("age", randint(18..99));

    //TRUE or FALSE (defaults to weight 0.5)
    column("active", randbool());
    
    //random(start..end) generates a random double within the range. If not limited goes up to DOUBLE_MAX
    column("score", randdouble(0.0..100.5));

    //format also supports padding e.g.: format({0000}, 10) -> 0010
    column("created_at", format("{0000}/{00}/{00}", randint(2000..2026), randint(1..12), randm(1..30)));

    //you can also put raw values into a column that will never change
    column("constant", "THIS NEVER CHANGES");
}
```
