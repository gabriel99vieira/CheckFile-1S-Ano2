<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/gabriel99vieira/CheckFile-1S-Ano2">
    <img src="https://upload.wikimedia.org/wikipedia/commons/9/9a/Log%C3%B3tipo_Polit%C3%A9cnico_Leiria_01.png" alt="Logo" width="200">
  </a>

  <h2 align="center">CheckFile</h2>

  <p align="center">
    Advanced programing (Language C)
    <br>
    Project carried out in the scope of Advanced programing subject
  </p>
</p>
<br />
<br />

<!-- ABOUT THE PROJECT -->

## About The Project

The main goal of this project is to check a file, directory or batch of files given as input and match its type with its extension to allow the user to know wich ones have the correct type.

<br>

## Usage

```
./checkfile
```

### Arguments

```
-f --file		Receives a file or multiple
-b --batch		Receives a 'txt' file with a list of files one per line
-d --dir		Receives a directory to loop through its files
```

File argument example

```
./checkfile -f file1.html -f file2.html
```

## Run stats

```
command time -v ./checkfile <args>
```

<br>

## Compiling

### Standard

```
make clean && make
```

### With debug

```
make clean && make debugon
```

### With executable ready

```
make clean && make && ./checkfile -f FILE
make clean && make && ./checkfile -b BATCH_FILE
make clean && make && ./checkfile -d POINT_DIRECTORY
```

## Contributors

Gabriel Madeira Vieira Nº2200661

Diogo dos Anjos Barbeiro Nº2200687

<br>

## Roadmap

[Roadmap Trello](https://trello.com/b/Jaw30Cxy/checkfile-1s-ano2)

<br>

<!-- LICENSE -->

## License

[MIT License](https://github.com/gabriel99vieira/CheckFile-1S-Ano2/blob/main/LICENSE)
