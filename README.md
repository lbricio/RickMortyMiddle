# Welcome to RickMortyMiddleware
> Essa aplicação é um middleware que consome a API [rickandmortyapi](https://rickandmortyapi.com/documentation/#get-multiple-characters).
> A aplicaçao também possui uma camada de cache local que armazena personagens, locais e episodios em modelos locais evitando overload excessivo na API consumida. Utiliza Conan como gerenciador de pacotes, CMake para automatizar o build, C++ como linguagem e Boost(Asio, Beast, JSON) como frameworks principais e Google Test para testes automatizados, além disso o projeto aplica boas práticas de divisão de responsabilidades e modularização de código.

### Endpoints:
`GET /help` visualiza todos os endpoints disponíveis
  
`GET /character/all`       retorna todos os personsagens em um único json;  
`GET /character/<id>`      retorna um personagem especifico pelo id;  
`GET /character/<id>,<id>` retorna varios personagens especificados por id;  
`GET /character/<?query>`  retorna personagens que cumprem o filtro especificado;  
  
`GET /episode/all`         retorna todos os episódios em um único json;  
`GET /episode/<id>`        retorna um episódio específico pelo id;  
`GET /episode/<id>,<id>`   retorna varios episódios por id;  
`GET /episode/<?query>`    retorna episódios a partir do filtro especificado; 
  
`GET /location/all`       retorna todas as localizações em um único json;  
`GET /location/<id>`      retorna uma localização especificada pelo id;  
`GET /location/<id>,<id>` retorna varias localizações especificadas por id;  
`GET /location/<?query>`  retorna localizações a partir do filtro especificado;  




### Stack:
| Tecnologia                          |  Descrição                                        |
| ----------------------------------- | ------------------------------------------------- |
| **C++20**                           | Linguagem principal do desafio                    |
| **Boost/Asio**                      | Networking (HTTP/HTTPS client + server)           |
| **Boost/Beast**                     | Engine HTTP e abstração de streams                |
| **Boost/JSON**                      | Parse e serialização de JSON                      |
| **GTest**                           | Testes unitários automatizados                    |
| **CMake**                           | Build system e automação de testes                |
| **Conan**                           | Gerenciador de dependências/pacotes               |
| **CMakeLists.txt**                  | Orquestra compilação e execução do test suite     |

### Estrutura do Projeto:
```
📁 RickMortyMiddle
├── 📁 include  
│   ├── api.hpp            Declara API do middleware + cache
│   ├── http_client.hpp    Interface do cliente HTTPS externo
│   ├── session.hpp        Sessão/Roteamento HTTP
│   ├── models.hpp         Models do domínio (Character, Episode e Location)
│   └── utils.hpp          Funções auxiliares
│  
├── 📁 src  
│   ├── api.cpp            Implementa consumo API externa + cache
│   ├── http_client.cpp    Implementa HTTPS (transporte)
│   ├── session.cpp        Controller HTTP (request, JSON response)
│   └── utils.cpp          Funções auxiliares
│  
├── 📁 tests  
│   ├── test_main.cpp      Inicializa GTest + testes unitários
│   └── test_endpoint.cpp  Testes de integração dos endpoints
│  
├── CMakeLists.txt         Orquestrador do build
├── conanfile.txt          Manifesto de dependências
└── CMakePresets.json      Configurações do CMake
```

---

# Install

### Package Manager

Instalação do Conan (caso não esteja disponível)
```
pip3 install --upgrade conan
conan profile detect
```

Verifique a instalação:
```
conan --version
```

### Build

1. Instalar dependências com Conan:
```
conan install . --output-folder=build --build=missing -s build_type=Release
```

2. Configurar o CMake usando o toolchain do Conan:
```
cmake -S . -B build/Release \
  -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release
```

3. Compilar:
```
cmake --build build/Release
```

4. Executar o Middleware
```
./build/Release/app
```
  
---
  
API
  
---
  
Decisões técnicas



