# FlixHabit

HeapFlix & Chill

https://flixhabit.netlify.app/

## Description

FlixHabit appears to be a project that analyzes relationships or data, possibly related to users and media content (like movies or shows), using graph algorithms and data structures. It features a C++ core for data processing and a React-based frontend for visualization, likely displaying the data as a force-directed graph. The name and tagline hint at a connection to recommendation systems or habit tracking within the context of media consumption.

The project utilizes graph structures (`Graph.h`, `Graph.cpp`) and min-heaps (`MinHeap.h`, `MinHeap.cpp`) in its C++ backend, suggesting sophisticated data analysis or pathfinding capabilities. User data (`User.h`) is likely incorporated into these analyses. JSON (`nlohmann/json.hpp`) is used for data serialization/deserialization, potentially for communication between the backend and frontend or for loading initial data.

The frontend ([frontend/](./frontend/)) uses React and `react-force-graph-2d` to visualize the processed data interactively, styled with Tailwind CSS.

## Features (Inferred)

*   **Data Processing:** Core logic implemented in C++ using graph algorithms and min-heaps.
*   **Data Visualization:** Interactive force-directed graph visualization using React.
*   **User Data Handling:** Incorporates user-related information in its analysis.
*   **JSON Support:** Reads or outputs data in JSON format.

## Tech Stack

*   **Backend/Core Logic:** C++20, CMake
*   **Libraries:** nlohmann/json (for C++ JSON handling)
*   **Frontend:** Node.js, React, `react-force-graph-2d`
*   **Styling:** Tailwind CSS, PostCSS, Autoprefixer
*   **Data:** Likely uses data files located in the [`data/`](./data/) directory coming from a kaggle dataset. 

## Getting Started

### Prerequisites

*   C++ Compiler supporting C++20 (e.g., GCC, Clang, MSVC)
*   CMake (version 3.27 or higher recommended)
*   Node.js and npm (or yarn)

### Installation & Setup

1.  **Clone the repository:**
    ```bash
    git clone <https://github.com/vincinious/FlixHabit.git>
    cd FlixHabit
    ```

2.  **Build the C++ Backend:**
    ```bash
    mkdir build
    cd build
    cmake ..
    make # or cmake --build .
    cd ..
    ```
    The executable `FlixHabit` should be created in the `build` directory (or `cmake-build-debug` if using an IDE like CLion).

3.  **Setup the Frontend:**
    ```bash
    cd frontend
    npm install # or yarn install
    cd ..
    ```

### Running the Application

*   **Backend:**
    Run the compiled C++ executable. You might need to provide input data files or arguments depending on its implementation.

*   **Frontend:**
    Navigate to the frontend directory and start the development server (assuming a standard React setup script exists in its `package.json`, which is not visible here - you might need to add one like `npm start`).
    ```bash
    cd frontend 
    cd flixhabit-frontend
    npm run dev
    ```
    Access the frontend via the URL provided by the development server (usually `http://localhost:5173`).

## Data

The project likely relies on data files located in the [`data/`](./data/) directory. Check this directory for sample data or instructions on data format.

## Testing

The C++ component includes tests in [`test/test.cpp`](./test/test.cpp). These can likely be run via CMake/CTest after building the project.

---
