// Copyright (c) 2019-2021 Fango Developers
// Copyright (c) 2018-2021 Fandom Gold Society
// Copyright (c) 2018-2019 Conceal Network & Conceal Devs
// Copyright (c) 2016-2019 The Karbowanec developers
// Copyright (c) 2012-2018 The CryptoNote developers
//
// This file is part of Fango.
//
// Fango is free software distributed in the hope that it
// will be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. You can redistribute it and/or modify it under the terms
// of the GNU General Public License v3 or later versions as published
// by the Free Software Foundation. Fango includes elements written 
// by third parties. See file labeled LICENSE for more details.
// You should have received a copy of the GNU General Public License
// along with Fango. If not, see <https://www.gnu.org/licenses/>.

#include "PasswordContainer.h"

#include <iostream>
#include <memory.h>
#include <stdio.h>

#if defined(_WIN32)
#include <io.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace Tools
{
  namespace
  {
    bool is_cin_tty();
  }

  PasswordContainer::PasswordContainer()
    : m_empty(true)
  {
  }

  PasswordContainer::PasswordContainer(std::string&& password)
    : m_empty(false)
    , m_password(std::move(password))
  {
  }

  PasswordContainer::PasswordContainer(PasswordContainer&& rhs)
    : m_empty(std::move(rhs.m_empty))
    , m_password(std::move(rhs.m_password))
  {
  }

  PasswordContainer::~PasswordContainer()
  {
    clear();
  }

  void PasswordContainer::clear()
  {
    if (0 < m_password.capacity())
    {
      m_password.replace(0, m_password.capacity(), m_password.capacity(), '\0');
      m_password.resize(0);
    }
    m_empty = true;
  }

  bool PasswordContainer::read_password()
  {
    clear();

    bool r;
    if (is_cin_tty())
    {
      std::cout << "password: ";
      r = read_from_tty();
    }
    else
    {
      r = read_from_file();
    }

    if (r)
    {
      m_empty = false;
    }
    else
    {
      clear();
    }

    return r;
  }

  bool PasswordContainer::read_from_file()
  {
    m_password.reserve(max_password_size);
    for (size_t i = 0; i < max_password_size; ++i)
    {
      char ch = static_cast<char>(std::cin.get());
      if (std::cin.eof() || ch == '\n' || ch == '\r')
      {
        break;
      }
      else if (std::cin.fail())
      {
        return false;
      }
      else
      {
        m_password.push_back(ch);
      }
    }

    return true;
  }

#if defined(_WIN32)

  namespace
  {
    bool is_cin_tty()
    {
      return 0 != _isatty(_fileno(stdin));
    }
  }

  bool PasswordContainer::read_from_tty()
  {
    const char BACKSPACE = 8;

    HANDLE h_cin = ::GetStdHandle(STD_INPUT_HANDLE);

    DWORD mode_old;
    ::GetConsoleMode(h_cin, &mode_old);
    DWORD mode_new = mode_old & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    ::SetConsoleMode(h_cin, mode_new);

    bool r = true;
    m_password.reserve(max_password_size);
    while (m_password.size() < max_password_size)
    {
      DWORD read;
      char ch;
      r = (TRUE == ::ReadConsoleA(h_cin, &ch, 1, &read, NULL));
      r &= (1 == read);
      if (!r)
      {
        break;
      }
      else if (ch == '\n' || ch == '\r')
      {
        std::cout << std::endl;
        break;
      }
      else if (ch == BACKSPACE)
      {
        if (!m_password.empty())
        {
          m_password.back() = '\0';
          m_password.resize(m_password.size() - 1);
          std::cout << "\b \b";
        }
      }
      else
      {
        m_password.push_back(ch);
        std::cout << '*';
      }
    }

    ::SetConsoleMode(h_cin, mode_old);

    return r;
  }

#else

  namespace
  {
    bool is_cin_tty()
    {
      return 0 != isatty(fileno(stdin));
    }

    int getch()
    {
      struct termios tty_old;
      tcgetattr(STDIN_FILENO, &tty_old);

      struct termios tty_new;
      tty_new = tty_old;
      tty_new.c_lflag &= ~(ICANON | ECHO);
      tcsetattr(STDIN_FILENO, TCSANOW, &tty_new);

      int ch = getchar();

      tcsetattr(STDIN_FILENO, TCSANOW, &tty_old);

      return ch;
    }
  }

  bool PasswordContainer::read_from_tty()
  {
    const char BACKSPACE = 127;

    m_password.reserve(max_password_size);
    while (m_password.size() < max_password_size)
    {
      int ch = getch();
      if (EOF == ch)
      {
        return false;
      }
      else if (ch == '\n' || ch == '\r')
      {
        std::cout << std::endl;
        break;
      }
      else if (ch == BACKSPACE)
      {
        if (!m_password.empty())
        {
          m_password.back() = '\0';
          m_password.resize(m_password.size() - 1);
          std::cout << "\b \b";
        }
      }
      else
      {
        m_password.push_back(ch);
        std::cout << '*';
      }
    }

    return true;
  }

#endif
}
