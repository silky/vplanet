Magnetic Braking
=================

Overview
--------

Rotational evolution of stars due to magnetic effects.

===================   ============
**Date**              10/21/18
**Author**            David Fleming
**Modules**           stellar
**Approx. runtime**   2 minutes
**Source code**       `GitHub <https://github.com/VirtualPlanetaryLaboratory/vplanet-private/tree/master/examples/MagneticBraking>`_
===================   ============

Rotation period evolution for 0.1 and 1.0 Msun stars due to stellar
evolution and magnetic braking.  We compare the how our different available
magnetic braking laws (Reiners & Mohanty (2012), Repetto & Nelemans (2014),
and Matt et al. (2015)) impact the rotation period evolution.


To run this example
-------------------

.. code-block:: bash

    python makeplot.py <pdf | png>


Expected output
---------------

.. figure:: MagneticBraking.png
   :width: 600px
   :align: center

   Rotation period evolution for a 0.1 and 1 Msun star due to stellar evolution
   (Baraffe et al. (2015)) and one of the 3 available magnetic braking laws.