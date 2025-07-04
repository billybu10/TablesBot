project('tablesBot', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++20'])

  dpp = dependency('dpp')
  incdir = include_directories('include', 'keys')

exe = executable('tablesBot', 'tablesBot.cpp',
  install : true,
  dependencies: [dpp],
  include_directories : incdir)

test('basic', exe)
