/* Proposed SG14 status_code
(C) 2018-2019 Niall Douglas <http://www.nedproductions.biz/> (5 commits)
File Created: Aug 2018


Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef BOOST_OUTCOME_SYSTEM_ERROR2_STD_ERROR_CODE_HPP
#define BOOST_OUTCOME_SYSTEM_ERROR2_STD_ERROR_CODE_HPP

#include "posix_code.hpp"

#if defined(_WIN32) || defined(BOOST_OUTCOME_STANDARDESE_IS_IN_THE_HOUSE)
#include "win32_code.hpp"
#endif

#include <system_error>

BOOST_OUTCOME_SYSTEM_ERROR2_NAMESPACE_BEGIN

namespace detail
{
  struct make_std_categories
  {
    static const std::error_category &generic_category() { return std::generic_category(); }
    static const std::error_category &system_category() { return std::system_category(); }
  };
}

template <class error_code_type, class make_category_types> class _error_code_domain;
//! A wrapper of `std::error_code`.
using std_error_code = status_code<_error_code_domain<std::error_code, detail::make_std_categories>>;

/*! The implementation of the domain for `std::error_code` error codes.
*/
template <class error_code_type, class make_categories_type> class _error_code_domain : public status_code_domain
{
  template <class DomainType> friend class status_code;
  template <class StatusCode> friend class detail::indirecting_domain;
  using _base = status_code_domain;
  using _status_code = status_code<_error_code_domain>;

  static _base::string_ref _make_string_ref(error_code_type c) noexcept
  {
    try
    {
      std::string msg = c.message();
      auto *p = static_cast<char *>(malloc(msg.size() + 1));  // NOLINT
      if(p == nullptr)
      {
        return _base::string_ref("failed to allocate message");
      }
      memcpy(p, msg.c_str(), msg.size() + 1);
      return _base::atomic_refcounted_string_ref(p, msg.size());
    }
    catch(...)
    {
      return _base::string_ref("failed to allocate message");
    }
  }

public:
  //! The value type of the `std::error_code` code, which is the `std::error_code`
  using value_type = error_code_type;
  using _base::string_ref;

  //! Default constructor
  constexpr explicit _error_code_domain(typename _base::unique_id_type id = 0x223a160d20de97b4) noexcept : _base(id) {}
  _error_code_domain(const _error_code_domain &) = default;
  _error_code_domain(_error_code_domain &&) = default;
  _error_code_domain &operator=(const _error_code_domain &) = default;
  _error_code_domain &operator=(_error_code_domain &&) = default;
  ~_error_code_domain() = default;

  //! Constexpr singleton getter. Returns constexpr error_code_domain variable.
  static inline constexpr const _error_code_domain &get();

  virtual string_ref name() const noexcept override { return string_ref("error_code compatibility domain"); }  // NOLINT
protected:
  virtual bool _do_failure(const status_code<void> &code) const noexcept override final  // NOLINT
  {
    assert(code.domain() == *this);
    return static_cast<const _status_code &>(code).value().value() != 0;  // NOLINT
  }
  virtual bool _do_equivalent(const status_code<void> &code1, const status_code<void> &code2) const noexcept override final  // NOLINT
  {
    assert(code1.domain() == *this);
    const auto &c1 = static_cast<const _status_code &>(code1);  // NOLINT
    const auto &cat1 = c1.value().category();
    // Are we comparing to another wrapped error_code?
    if(code2.domain() == *this)
    {
      const auto &c2 = static_cast<const _status_code &>(code2);  // NOLINT
      const auto &cat2 = c2.value().category();
      // If the error code categories are identical, do literal comparison
      if(cat1 == cat2)
      {
        return c1.value().value() == c2.value().value();
      }
      // Otherwise fall back onto the _generic_code comparison, which uses default_error_condition()
      return false;
    }
    // Am I an error code with generic category?
    const auto &generic_category = make_categories_type::generic_category();
    if(cat1 == generic_category)
    {
      // Convert to generic code, and compare that
      generic_code _c1(static_cast<errc>(c1.value().value()));
      return _c1 == code2;
    }
    // Am I an error code with system category?
    const auto &system_category = make_categories_type::system_category();
    if(cat1 == system_category)
    {
// Convert to POSIX or Win32 code, and compare that
#ifdef _WIN32
      win32_code _c1((win32::DWORD) c1.value().value());
#else
      posix_code _c1(c1.value().value());
#endif
      return _c1 == code2;
    }
    return false;
  }
  virtual generic_code _generic_code(const status_code<void> &code) const noexcept override final  // NOLINT
  {
    assert(code.domain() == *this);
    const auto &c = static_cast<const _status_code &>(code);  // NOLINT
    // Ask my embedded error code for its mapping to std::errc, which is a subset of our generic_code errc.
    return generic_code(static_cast<errc>(c.value().default_error_condition().value()));
  }
  virtual string_ref _do_message(const status_code<void> &code) const noexcept override  // NOLINT
  {
    assert(code.domain() == *this);
    const auto &c = static_cast<const _status_code &>(code);  // NOLINT
    return _make_string_ref(c.value());
  }
#if defined(_CPPUNWIND) || defined(__EXCEPTIONS) || defined(BOOST_OUTCOME_STANDARDESE_IS_IN_THE_HOUSE)
  BOOST_OUTCOME_SYSTEM_ERROR2_NORETURN virtual void _do_throw_exception(const status_code<void> &code) const override  // NOLINT
  {
    assert(code.domain() == *this);
    const auto &c = static_cast<const _status_code &>(code);  // NOLINT
    throw std::system_error(c.value());
  }
#endif
};
//! A constexpr source variable for the `std::error_code` code domain. Returned by `_error_code_domain<error_code_type, detail::make_std_categoriesy>::get()`.
constexpr _error_code_domain<std::error_code, detail::make_std_categories> std_error_code_domain;
template <class error_code_type, class make_categories_type> inline constexpr const _error_code_domain<error_code_type, make_categories_type> &_error_code_domain<error_code_type, make_categories_type>::get()
{
  return std_error_code_domain;
}
// Enable implicit construction of `std_error_code` from `std::error_code`.
inline std_error_code make_status_code(std::error_code c) noexcept
{
  return std_error_code(in_place, c);
}

BOOST_OUTCOME_SYSTEM_ERROR2_NAMESPACE_END

#endif
