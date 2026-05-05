import pathlib
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[1]


class ThirdPartyPolicyTest(unittest.TestCase):
    def test_etl_is_managed_by_pixi_not_cmake_fetchcontent(self):
        cxx_cmake = (ROOT / "cmake" / "rrtos_cxx.cmake").read_text()
        pixi_toml = (ROOT / "pixi.toml").read_text()
        recipe = (ROOT / "packages" / "etlcpp" / "recipe.yaml")

        self.assertNotIn("FetchContent", cxx_cmake)
        self.assertNotIn("GIT_REPOSITORY", cxx_cmake)
        self.assertNotIn("RRTOS_ETL_SOURCE_DIR", cxx_cmake)
        self.assertIn("find_package(etl CONFIG REQUIRED)", cxx_cmake)

        self.assertTrue(recipe.exists(), "ETLCPP must be represented as a Pixi local package")
        recipe_text = recipe.read_text()
        self.assertIn('name: "etlcpp"', recipe_text)
        self.assertIn("https://github.com/ETLCPP/etl/archive/refs/tags/", recipe_text)
        self.assertIn('etlcpp = { path = "packages/etlcpp" }', pixi_toml)


if __name__ == "__main__":
    unittest.main()
