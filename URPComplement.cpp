#include <iostream>
#include <vector>
#include <stdexcept>
#include <limits>
#include <algorithm>


// breakpoint macro
#ifdef DEBUG_BREAKPOINT
  #if defined(_MSC_VER)
    #define BREAKPOINT __debugbreak()
  #elif defined(__APPLE__)
    #define BREAKPOINT __builtin_debugtrap()
  #elif defined(__linux__)
    #define BREAKPOINT raise(SIGTRAP)
  #else
    #define BREAKPOINT raise(SIGTRAP)
  #endif
#else
  #define BREAKPOINT ((void)0)  // No-op when not debugging
#endif

// debug macros
#ifdef DEBUG_PRINT
template <class... Args>
inline void DEBUG(Args&&... args) {
    (std::cout << ... << args) << '\n';
}
#else
// compiled out
template <class... Args>
inline void DEBUG(Args&&...) {}
#endif


#ifdef DEBUG_PRINT
  #define PRINT_CUBE_LIST(cubeList) printCubeList(cubeList) 
#else
  #define PRINT_CUBE_LIST(cubeList) ((void)0)
#endif



// typedef uint8_t cv_t;
using cv_t = uint8_t;
enum class cubeVar_t : cv_t {
  O = 01,
  Z = 10,
  A = 11, // absent
};

using cube_t = std::vector<cubeVar_t>;
using cubeList_t = std::vector<cube_t>;
using unateVarStats_t = std::vector<std::tuple<bool, cubeVar_t, size_t, int>>;


// global variables
size_t numVars = 0; // TODO replace the throws in other functions when this was needed

/**
 * @brief Prints the cubes
 * 
 * @param cubes 
 */
static void printCubeList(cubeList_t& cubes) noexcept;


/**
 * @brief Populates the unate stats for each of the vars
 * unateVarStats_t:
 * 1) bool -> wether or not it's unate
 * 2) cubeVar_t -> used for computation. If it's unate, this 
 *    holds the type of unate (pos of neg), f_x is
 * 3) size_t -> the binateCount (only valid if it's not unate)
 * 4) int -> the compliment difference (will be positive, only
 *     int needed during the computation).
 * 
 * @param cubes 
 * @return std::pair<bool, unateVarStats_t> 
 */
[[nodiscard]] static std::pair<bool, unateVarStats_t> populateVarStats(const cube_t& cubes);


/**
 * @brief Returns a cube list in SOP PCN form after
 * performing deMorgan's on a single PNC vector.
 * 
 * @param cube 
 * @return cube_t 
 */
cubeList_t deMorgan(const cube_t& cube) noexcept;


/**
 * @brief Ands cub1 and cube2 into cube2.
 * This is not a general purpose and. It's expected that
 * cube1 represents a cofactor of cube2. And thus, if
 * cube1 is not a cofactor, an expression is thrown.
 * 
 * 
 * @param cube1 
 * @param cube2 the modified cube
 */
static void andCubes(const cube_t& cube1, cube_t& cube2);

/**
 * @brief Ands the cube list with it's cofactor
 * 
 * @param pos 
 * @param var 
 * @param cubeList
 */
static void andCubeListAndCube(size_t pos, cubeVar_t cubeVar, cubeList_t& cubeList);


/**
 * @brief Non minimal or-ing of two cube list.
 * Simply concatenates the cubList2 at the end of cubeList1.
 * 
 * @param cubeList1 
 * @param cubeList2 
 */
static void orCubeLists(cubeList_t& cubeList1, const cubeList_t& cubeList2) noexcept;


/**
 * @brief Returns if the cubeList contains an 
 * all don't care cube (cubeVar_t::A)
 * 
 * @param cubeList 
 * @return true 
 * @return false 
 */
[[nodiscard ]]static bool contains1(const cubeList_t& cubeList) noexcept;


/**
 * @brief Returns a cofactored cube list.
 * Polarity:
 *  Positive if cubeVar == cubeVar_t::O
 *  Negative if cubeVar == cubeVar_t::Z
 * 
 * 
 * @param pos 
 * @param cubeVar 
 * @param cubeList 
 * @return cubeList_t 
 */
static cubeList_t _cofactor(size_t pos, cubeVar_t cubeVar, const cubeList_t& cubeList) noexcept;
/**
 * @brief Returns cofactors of cubeList about the pos variable
 * std::pair<cubeList_t, cubeList_t> = <negative cofactor, positive cofactor>
 * 
 * @param pos 
 * @param cubeList 
 * @return std::pair<cubeList_t, cubeList_t> 
 */
static std::pair<cubeList_t, cubeList_t> cofactor(size_t pos, const cubeList_t& cubeList) noexcept;


/**
 * @brief Determine the cofactor index based on the selection algorithm
 * 
 * @param unStats 
 * @return size_t 
 */
[[nodiscard]] static size_t selectCofactorIndex(const std::pair<bool, unateVarStats_t>& unStats) noexcept;


/**
 * @brief The main URP recursive algorithm
 * 
 * @param cubeList 
 * @return cubeList_t 
 */
[[nodiscard]] cubeList_t compliment(const cubeList_t& cubeList);




int main() {
  /* parsing input file */
  DEBUG("-----PARSING-----");
  size_t numCubes(0);



  std::cin >> numVars >> numCubes;
  cubeList_t cubeList(numCubes, cube_t(numVars, cubeVar_t::A));

  int var(0);
  for (size_t i = 0; i < numCubes; ++i) {
    std::cin >> numVars;
    DEBUG("list: ", i, " and vars: ", numVars);
    for (size_t j = 0; j < numVars; ++j) {
      std::cin >> var;
      if (var < 0) {
        cubeList[i][(-var) - 1] = cubeVar_t::Z;
      } else {
        cubeList[i][(var) - 1] = cubeVar_t::O;
      }
    }
  }


  DEBUG("Initial cube list after parsing: ");
  PRINT_CUBE_LIST(cubeList);

  DEBUG("\n\n");
  BREAKPOINT;


  cubeList_t complimentCubeList = compliment(cubeList);
  DEBUG("-----RESULTING CUBE-----");
  PRINT_CUBE_LIST(complimentCubeList);

} // main()


static void printCubeList(cubeList_t& cubeList) noexcept {
    for (auto &cube: cubeList) {
    for (auto &var: cube) {
      switch(var) {
        case (cubeVar_t::O):
          std::cout << "01";
          break;
        case (cubeVar_t::Z):
          std::cout << "10";
          break;
        case (cubeVar_t::A):
          std::cout << "11";
          break;
        default :
          std::cout << "NA";
          break;
        }
        std::cout << ",";
    }
    std::cout << std::endl;
  }
} // printCubeList()



[[nodiscard]] static std::pair<bool, unateVarStats_t> populateVarStats(const cubeList_t& cubeList) {
  if (cubeList.empty()) {
    throw std::logic_error("Called populateVarStats() with empty cubeList");
  }
  bool allVarsUnate = true;
  unateVarStats_t unateVarStats(numVars, {true, cubeVar_t::A, 0, 0});
  for(auto& cube: cubeList) {
    for (int i = 0; i < cube.size(); ++i){
      bool& isUnate = std::get<0>(unateVarStats[i]);
      cubeVar_t& cubeVar = std::get<1>(unateVarStats[i]);
      size_t& binateCnt = std::get<2>(unateVarStats[i]);
      int& cnt = std::get<3>(unateVarStats[i]);

      // update logic state-machine
      if (cubeVar == cubeVar_t::A && cube[i] != cubeVar_t::A) {
        cubeVar = cube[i];
      } else if (cubeVar != cube[i]) {
        isUnate = false;
        allVarsUnate = false;
      }

      binateCnt += (cube[i] == cubeVar_t::A) ? 0 : 1;
      cnt += (cube[i] == cubeVar_t::A) ? 0 
        : (cube[i] == cubeVar_t::Z) ? -1 : 1;
    }
  }

  // make the cnts pos
  std::for_each(unateVarStats.begin(), unateVarStats.end(), 
    [](auto& varStats) -> void {
      std::get<3>(varStats) = std::abs(std::get<3>(varStats));
    });
  return std::pair<bool, unateVarStats_t>{allVarsUnate, unateVarStats};
} // populateVarStats()


cubeList_t deMorgan(const cube_t& cube) noexcept {
  cubeList_t res;
  for (int i = 0; i < cube.size(); ++i) {
    if (cube[i] != cubeVar_t::A) {
      res.push_back(cube_t(cube.size(), cubeVar_t::A));
      res.back()[i] = cube[i] == cubeVar_t::Z ? cubeVar_t::O : cubeVar_t::Z;
    }
  }
  return res;
} // deMorgan()


static void andCubes(const cube_t& cube1, cube_t& cube2) {
  BREAKPOINT;
  // report non-don't care issue
  if (cube1.size() != cube2.size()) {
    throw std::logic_error("Cube sizes for anding don't match");
  }
  for (int i = 0; i < cube1.size(); ++i) {
    if (cube1[i] != cubeVar_t::A) {
      if (cube2[i] != cubeVar_t::A) {
        throw std::logic_error("Anding two non-don't cares");
      } else {
        // only modifieng cube2
        cube2[i] = cube1[i];
      }
    }
  }
} // andCubes()


static void andCubeListAndCube(size_t pos, cubeVar_t cubeVar, cubeList_t& cubeList) {
  if (cubeList.empty()) {
    throw std::logic_error("Calling varCubeListAnd() with empty cube");
  }
  cube_t varPCN(numVars, cubeVar_t::A);
  varPCN[pos] = cubeVar;
  for (auto& cube: cubeList) {
    andCubes(varPCN, cube);
  }
} // andCubeListAndCube()

static void orCubeLists(cubeList_t& cubeList1, const cubeList_t& cubeList2) noexcept {
  cubeList1.insert(cubeList1.end(), cubeList2.begin(), cubeList2.end());
} // orCubeLists()


[[nodiscard ]]static bool contains1(const cubeList_t& cubeList) noexcept {
  return std::any_of(cubeList.begin(), cubeList.end(), 
  [](const auto& cube) -> bool {
    return std::all_of(cube.begin(), cube.end(), 
    [](const auto& cubeVar) -> bool {
      return cubeVar == cubeVar_t::A;
    });
  });
} // contains1()

static std::pair<cubeList_t, cubeList_t> cofactor(size_t pos, const cubeList_t& cubeList) noexcept {
  return std::pair(_cofactor(pos, cubeVar_t::Z, cubeList), _cofactor(pos, cubeVar_t::O, cubeList));
} // cofactor()


static cubeList_t _cofactor(size_t pos, cubeVar_t cubeVar, const cubeList_t& cubeList) noexcept {
  cubeList_t cofactorCubeList;
  std::for_each(cubeList.begin(), cubeList.end(), 
    [&cofactorCubeList, pos, cubeVar](const auto& cube) {
      switch (cube[pos]) {
        case (cubeVar_t::A) :
          cofactorCubeList.push_back(cube);
          break;
        case (cubeVar_t::Z) :
          if (cubeVar == cubeVar_t::Z) {
            cofactorCubeList.push_back(cube);
            cofactorCubeList.back()[pos] = cubeVar_t::A;
          }
          break;
        case (cubeVar_t::O) :
          if (cubeVar == cubeVar_t::O) {
            cofactorCubeList.push_back(cube);
            cofactorCubeList.back()[pos] = cubeVar_t::A;
          }
        default: break;
      }
  });
  return cofactorCubeList;
} // _cofactor()


[[nodiscard]] static size_t selectCofactorIndex(const std::pair<bool, unateVarStats_t>& unStats) noexcept {
  size_t cofactorIndex = 0, i = 0;

  auto [allVarsUnate, unateVarStats] = unStats;
  
  // 1) If there's binate varibles
  if (!allVarsUnate) {
    size_t maxBinateCnt = 0,
      maxBinateIndex = 0,
      minCompDiffCnt = std::numeric_limits<size_t>::max(),
      minCompDiffIndex = 0;
    bool singleMax = true;
    std::for_each(unateVarStats.cbegin(), unateVarStats.cend(), 
      [&](const auto& uvs){
      const bool isUnate = std::get<0>(uvs);
      const cubeVar_t cubeVar = std::get<1>(uvs);
      const size_t binateCnt = std::get<2>(uvs);
      const int cnt = std::get<3>(uvs);

      if (!isUnate && binateCnt > maxBinateCnt) {
        maxBinateCnt = binateCnt;
        singleMax = true;
        maxBinateIndex = i;
      } else if ((binateCnt == maxBinateCnt)) {
        singleMax = false;
      }

      if (!isUnate && cnt < minCompDiffCnt) {
        minCompDiffIndex = i;
        minCompDiffCnt = cnt;
      }

      ++i;
    });

    // 1a) found a single max to cofactoer
    if (singleMax) {
      cofactorIndex = maxBinateIndex;
    } else {
      cofactorIndex = minCompDiffIndex;
    }
  } 
  // 2) All unate variables
  else {
    size_t maxAppearsCnt = 0, maxAppearsIndex = 0;
    std::for_each(unateVarStats.cbegin(), unateVarStats.cend(), 
    [&maxAppearsCnt, &maxAppearsIndex, &i](const auto& uvs){
      const size_t binateCnt = std::get<2>(uvs);
      if (binateCnt > maxAppearsCnt) {
        maxAppearsIndex = i;
        maxAppearsCnt = binateCnt;
      }
      ++i;
    });
    cofactorIndex = maxAppearsIndex;
  }
  return cofactorIndex;
} // selectCofactorIndex()

[[nodiscard]] cubeList_t compliment(const cubeList_t& cubeList) {
  /* Check termination */

  // 1) if all empty, return an all don't cares cube representing a 1
  if (cubeList.empty()) {
    return cubeList_t(1, cube_t(numVars, cubeVar_t::A));
  }

  // 2) if contains and all 1s, it's a tautology,
  //    eturn an empty cubeList representing a 0
  if (contains1(cubeList)) {
    return cubeList_t(); // TODO: check some of the empty conditions
  }

  // 1) if just one cube, return it's compliment using deMorgans
  if (cubeList.size() == 1) {
    return deMorgan(cubeList[0]);
  }

  // -- Need to recurse further
  BREAKPOINT;

  /* Selection Crieteria */
  size_t cofactorIndex = selectCofactorIndex(populateVarStats(cubeList));
  
  BREAKPOINT;

  /* Perform recursion and return */
  auto [negCofactor, posCofactor] = cofactor(cofactorIndex, cubeList);

  negCofactor = compliment(negCofactor);
  posCofactor = compliment(posCofactor);

  BREAKPOINT;

  andCubeListAndCube(cofactorIndex, cubeVar_t::Z, negCofactor);
  andCubeListAndCube(cofactorIndex, cubeVar_t::O, posCofactor);
  // concatenate neg after pos
  orCubeLists(posCofactor, negCofactor);

  BREAKPOINT;

  return posCofactor;
} // compliment()