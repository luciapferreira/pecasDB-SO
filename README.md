# Exercise 1 for Operating Systems
In many constrained environments, for instance a satellite, it is necessary that a product contains a description of itself. Obviously knowing a parts id and manufacturing company (brands) id is not sufficient, one must consult a database to get the actual part and brand details.

The database file in this case is a structured binary file containing an array of records. The database file (example: file.pecas) consists of a header followed by a section detailing the brands (manufacturing companies) then an array of records of type Part (Peca) ordered by id or ordered by name.

The header consists of 28 Bytes. The first byte 20 bytes are a string, the first character T to indicate the type of ordering of the parts (the character 0,1) to indicate if the parts are ordered by id (T=’0’) or by name (T=’1’). 
This is followed by 19 bytes for a string description. Next are two binary encoded integers (4 bytes each) the number of parts (NP) and the number of brands (NB).
The next section consists of a database of brands ordered as a set of brands. Each brand is a string of at most 16 characters including the string terminator.

The program should accept the following options entered via standard input : 1,2,3,4,5 ou 9 with the
following semantics.

- 1 print the header

- 9 to exit
  
When the database file is ordered by ID the options 2,3, and 4 are also valid
- 2 view a part with id
- 3 add a new part specifying (in this order) name brand id weight and price – the id can be added automatically.
- 4 to change a part with id specifying (in this order) a new name brand id weight and price
  
When the database file of parts is ordered by part NAME only the further option 5 is valid
- 5 to view a part with name
