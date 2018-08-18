:mod:`uerrno` -- system error codes
===================================

.. include:: ../templates/unsupported_in_circuitpython.inc

.. module:: uerrno
   :synopsis: system error codes

|see_cpython_module| :mod:`cpython:errno`.

This module provides access to symbolic error codes for `OSError` exception.

Constants
---------

.. data:: EEXIST, EAGAIN, etc.

    Error codes, based on ANSI C/POSIX standard. All error codes start with
    "E". Errors are usually accessible as ``exc.args[0]``
    where ``exc`` is an instance of `OSError`. Usage example::

        try:
            os.mkdir("my_dir")
        except OSError as exc:
            if exc.args[0] == uerrno.EEXIST:
                print("Directory already exists")

.. data:: errorcode

    Dictionary mapping numeric error codes to strings with symbolic error
    code (see above)::

        >>> print(uerrno.errorcode[uerrno.EEXIST])
        EEXIST
