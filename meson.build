project('PrePress', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])


adw = dependency('libadwaita-1')

blueprints = custom_target('blueprints',
  input: files(
    'ui/main.blp'
  ),
  output: '.',
  command: [find_program('blueprint-compiler'), 'batch-compile', '@OUTPUT@', '@CURRENT_SOURCE_DIR@', '@INPUT@'],
)

exe = executable('prepress', ['prepress.c', 'export.c'],
  install : true, dependencies: [adw], link_depends: [blueprints])

test('basic', exe)
