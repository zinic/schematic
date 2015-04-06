import os
import sys

from setuptools import setup, find_packages
from distutils.extension import Extension
from Cython.Build import cythonize


def read(relative):
    contents = open(relative, 'r').read()
    return [l for l in contents.split('\n') if l != '']


def compile_pyx():
    # chdir so that cython is happy
    os.chdir('src/python')
    return cythonize('schematic/*.pyx')


setup(
    name='schematic',
    version='0.0.1',
    description='Programming language fun',
    author='John Hopper',
    author_email='john.hopper@jpserver.net',
    url='https://github.com/zinic/schematic',
    license='MIT',
    classifiers=[
        'Development Status :: 4 - Beta',
        'Environment :: Other Environment',
        'Natural Language :: English',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python',
        'Programming Language :: Cython',
        'Programming Language :: Python :: Implementation :: CPython',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3.3',
        'Topic :: Utilities'
    ],
    scripts=['src/scripts'],
    tests_require=read('project/tests-require.txt'),
    install_requires=read('project/install-requires.txt'),
    test_suite='nose.collector',
    zip_safe=False,
    include_package_data=True,
    packages=find_packages(exclude=['*tests*']),
    extra_compile_args=['-O3'],
    ext_modules=compile_pyx())
