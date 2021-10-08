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

The main goal of this project is to evaluate a file type or a given batch of files.
<br>
To make sure that every one has the right extension.

<br>

<!-- GETTING STARTED -->

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
make clean && make && ./checkfile -d POINT_DIRECTORY -f FILE

make clean && make && ./checkfile -b BATCH_FILE
make clean && make && ./checkfile -d POINT_DIRECTORY -b BATCH_FILE
```

## Usage

> Help

```
./checkfile -h
```

> Execute

```
./checkfile -f FILE
./checkfile -f FILE -d POINT_DIRECTORY

./checkfile -b FILE
./checkfile -b FILE -d POINT_DIRECTORY
```

<br>

## Work by

Gabriel Madeira Vieira Nº2200661

Diogo dos Anjos Barbeiro Nº???????

<br>

## Roadmap

[Roadmap Trello](https://trello.com/b/Jaw30Cxy/checkfile-1s-ano2)

<br>

<!-- LICENSE -->

## License

Distributed under the MIT License. See `LICENSE` for more information.
