library(simplr)
context("mathematical expression simplification via simplify()")

s <- simplifyq
u <- quote

test_that("constant folding works", {
  expect_identical(s(1+1), 2)
  expect_identical(s(1*1), 1)
  expect_identical(s(3+2+1), 6)
  expect_identical(s(3*2*1), 6)
  expect_identical(s(3*2+1), 7)
  expect_identical(s(5+5+5*5), 35)

  expect_equal(s(sin(3.141593/2)+2), 3)
})

test_that("basic simplification works", {
  expect_identical(s(1 * x), u(x))
  expect_identical(s(x / x), u(1))
  expect_identical(s(x - x), u(0))
  expect_identical(s(x + 1 - 1), u(x))
  expect_identical(s(f(x) + f(y)), u(f(x) + f(y)))
  expect_identical(s(f(x) + f(x)), u(2 * f(x)))
})

test_that("advanced simplification works", {
  expect_identical(s(0/y^3 + 0/y^3), u(0))
  expect_equal(s(1/y^3 + 1/y^3), u(2 * (1/y^3)))
  expect_equal(s(2/y^3 + 2/y^3), u(2 * (2/y^3)))
  expect_equal(s(1/y^3 + 2/y^3), u(1/y^3 + 2/y^3))
  expect_equal(s(2/y^3 + 1/y^3), u(2/y^3 + 1/y^3))
  expect_equal(s(1/y^3 + 3/y^3), u(1/y^3 + 3/y^3))
  expect_equal(s(3/y^3 + 1/y^3), u(3/y^3 + 1/y^3))
  expect_equal(s(3/(y^3) + 1/(y^3)), u(3/y^3 + 1/y^3))
  expect_equal(s(1/y^3 + 10/y^3), u(1/y^3 + 10/y^3))
  expect_equal(s(10/y^3 + 1/y^3), u(10/y^3 + 1/y^3))
  expect_identical(s(1/y^3 - 1/y^3), u(0))
  expect_equal(s(1/y^3 - 2/y^3), u(1/y^3 + -(2/y^3)))
})

test_that("trigonometric simplification works", {
  expect_identical(s(sin(x)^2+cos(x)^2), u(1))
})
